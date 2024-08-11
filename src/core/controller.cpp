#include <controller/seadController.h>
#include <controller/seadControllerMgr.h>

#include "core/controller.hpp"
#include "util/message.hpp"
#include "util/named.h"

namespace botw::savs {

bool Controller::initialize() {
    if (m_controller) {
        return true;
    }

    auto* controller_mgr = sead::ControllerMgr::instance();
    if (!controller_mgr) {
        return false;
    }
    auto* controller = controller_mgr->getController(0);
    if (!controller) {
        return false;
    }

    m_controller = controller;
    return true;
}

void Controller::save_key_bindings(DataWriter& w) const {
    w.write_integer(_named(m_key_save));
    w.write_integer(_named(m_key_save_file));
    w.write_integer(_named(m_key_restore));
    w.write_integer(_named(m_key_restore_file));
}

void Controller::load_key_bindings(DataReader& r) {
    u32 key_save = 0;
    u32 key_save_file = 0;
    u32 key_restore = 0;
    u32 key_restore_file = 0;
    r.read_integer(&key_save);
    r.read_integer(&key_save_file);
    r.read_integer(&key_restore);
    r.read_integer(&key_restore_file);

    if (r.is_successful()) {
        m_key_save = static_cast<Key>(key_save);
        m_key_save_file = static_cast<Key>(key_save_file);
        m_key_restore = static_cast<Key>(key_restore);
        m_key_restore_file = static_cast<Key>(key_restore_file);
    }
}

bool Controller::is_only_holding(Key keys) const {
    return m_controller->isHoldAll(keys) && !m_controller->isHold(~keys);
}

Key Controller::get_hold_keys() const {
    return static_cast<Key>(m_controller->getHoldMask());
}

bool Controller::has_held_keys_for(Key keys, u32 seconds) {
    if (m_holding_keys != keys) {
        m_holding_keys = keys;
        m_hold_counter = 0;
    } else {
        m_hold_counter++;
    }
    // 1 tick = 3 frames
    // 1s = 30 frames = 10 ticks
    u32 ticks = seconds * 10;
    if (m_hold_counter >= ticks) {
        m_hold_counter = 0;
        return true;
    }
    return false;
}

void Controller::start_configure_key(Key* key) {
    clear_hold();
    StringBuffer<30> key_name;
    get_key_name(key_name, key);
    msg::show_customf("Hold new %s key for 3 seconds", key_name.content());
    m_key_being_configured = key;
    m_configure_hold_counter = 0;
}

Controller::ConfigureResult Controller::finish_configure_key(Key new_key) {
    if (m_configure_hold_counter == 0) {
        m_holding_keys = new_key;
    } else {
        if (m_holding_keys != new_key) {
            m_configure_hold_counter = 0;
            return ConfigureResult::Pending;
        }
    }
    if (new_key == 0) {
        m_configure_hold_counter = 0;
        return ConfigureResult::Pending;
    }
    if (m_configure_hold_counter < 30) {
        m_configure_hold_counter++;
        return ConfigureResult::Pending;
    }

    clear_hold();
    if (new_key == 0) {
        m_key_being_configured = nullptr;
        return ConfigureResult::FailEmpty;
    }
    *m_key_being_configured = new_key;
    return ConfigureResult::Success;
}

Command Controller::update() {
    // handle mode switch input
    if (is_only_holding(KEY_SWITCH_MODE)) {
        if (has_held_keys_for(KEY_SWITCH_MODE, 3)) {
            if (m_mode == Mode::Active) {
                m_mode = Mode::Setting;
            } else {
                m_mode = Mode::Active;
            }
            clear_hold();
            return Command::SwitchMode;
        }
        return Command::None;
    }

    if (m_mode == Mode::Setting) {
        return update_setting_mode();
    }

    return update_active_mode();
}

Command Controller::update_setting_mode() {
    if (is_configuring_key()) {
        Key new_key = get_hold_keys();
        switch (finish_configure_key(new_key)) {
            case Controller::ConfigureResult::Success: {
                StringBuffer<30> key_name;
                get_key_name(key_name, m_key_being_configured);
                StringBuffer<120> buffer;
                get_key_string(new_key, buffer);
                StringBuffer<200> msg;
                msg.appendf("%s key is now set to ", key_name.content());
                msg.append(buffer.content());
                msg::show_custom(msg.content());
                m_key_being_configured = nullptr;
                return Command::SaveOption;
            }
            case Controller::ConfigureResult::FailEmpty:
                msg::show_custom("Key binding cannot be updated to none!");
                return Command::None;
            default:
                return Command::None;
        }
    }

    if (is_only_holding(KEY_INCREASE_LEVEL)) {
        if (has_held_keys_for(KEY_INCREASE_LEVEL, 1)) {
            return Command::IncreaseLevel;
        }
    } else if (is_only_holding(KEY_DECREASE_LEVEL)) {
        if (has_held_keys_for(KEY_DECREASE_LEVEL, 1)) {
            return Command::DecreaseLevel;
        }
    } else if (is_only_holding(m_key_save)) {
        if (has_held_keys_for(m_key_save, 3)) {
            start_configure_key(&m_key_save);
        }
    } else if (is_only_holding(m_key_save_file)) {
        if (has_held_keys_for(m_key_save_file, 3)) {
            start_configure_key(&m_key_save_file);
        }
    } else if (is_only_holding(m_key_restore)) {
        if (has_held_keys_for(m_key_restore, 3)) {
            start_configure_key(&m_key_restore);
        }
    } else if (is_only_holding(m_key_restore_file)) {
        if (has_held_keys_for(m_key_restore_file, 3)) {
            start_configure_key(&m_key_restore_file);
        }
    } else {
        clear_hold();
    }

    return Command::None;
}

Command Controller::update_active_mode() {
    if (m_restore_fired) {
        // after restore, keep firing PostRestoreHold
        // until all keys are released
        if (get_hold_keys() != Key::None) {
            return Command::PostRestoreHold;
        }
        m_restore_fired = false;
        return Command::RestoreDone;
    }
    if (is_only_holding(m_key_save)) {
        return Command::Save;
    } else if (is_only_holding(m_key_save_file)) {
        return Command::SaveFile;
    } else if (is_only_holding(m_key_restore)) {
        m_restore_fired = true;
        return Command::Restore;
    } else if (is_only_holding(m_key_restore_file)) {
        m_restore_fired = true;
        return Command::RestoreFile;
    }
    return Command::None;
}


}

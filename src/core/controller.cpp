#include <controller/seadController.h>
#include <controller/seadControllerMgr.h>
#include <toolkit/mem/named_value.hpp>
#include <toolkit/msg/info.hpp>
#include <toolkit/msg/widget.hpp>
#include <toolkit/tcp.hpp>

#include "core/controller.hpp"

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

void Controller::save_key_bindings(io::DataWriter& w) const {
    w.write_integer(_named(m_key_save));
    w.write_integer(_named(m_key_save_file));
    w.write_integer(_named(m_key_restore));
    w.write_integer(_named(m_key_restore_file));
}

void Controller::load_key_bindings(io::DataReader& r) {
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
    m_key_being_configured = key;
    show_configuring_key_message();
    m_configure_hold_counter = 0;
}

bool Controller::finish_configure_key(Key new_key) {
    if (m_configure_hold_counter == 0) {
        m_holding_keys = new_key;
    } else {
        if (m_holding_keys != new_key) {
            m_configure_hold_counter = 0;
            return false;
        }
    }
    if (new_key == 0) {
        m_configure_hold_counter = 0;
        return false;
    }
    if (m_configure_hold_counter < 30) {
        m_configure_hold_counter++;
        if (m_configure_hold_counter % 10 == 0) {
            mem::StringBuffer<120> buffer;
            get_key_string(new_key, buffer);
            msg::info::printf("Keep holding %s", buffer.content());
        }
        return false;
    }

    clear_hold();
    *m_key_being_configured = new_key;
    return true;
}

Command Controller::update() {
    // handle mode switch input
    if (is_only_holding(KEY_SETTINGS)) {
        if (has_held_keys_for(KEY_SETTINGS, 2)) {
            m_mode = Mode::SettingHome;
            m_menu_current_option = 0;
            m_tick_since_last_menu_input = MENU_REFRESH_TICKS + 1;
            clear_hold();
            return Command::SwitchMode;
        }
        return Command::None;
    }

    if (m_mode == Mode::Active) {
        return update_active_mode();
    }
    return update_setting_mode();
}

static const char* get_enabled_text(bool value) {
    return value ? "Enabled" : "Disabled";
}

static const char* get_toggle_text(bool value) {
    return value ? "On" : "Off";
}

Command Controller::update_setting_mode() {
    m_tick_since_last_menu_input++;
    // show the menu again after the widget has (approximately) disappeared
    if (m_tick_since_last_menu_input >= MENU_REFRESH_TICKS) {
        refresh_menu();
    }
    // throttle menu input
    if (m_tick_since_last_menu_input < MENU_INPUT_INTERVAL) {
        return Command::None;
    }
    // explain message
    if (m_menu_showing_explain_message) {
        // pressing B can close the explain message
        if (is_only_holding(Key::B)) {
            m_menu_showing_explain_message = false;
            refresh_menu();
            return Command::None;
        }
    }
    // menu option selection
    if (is_only_holding(Key::DpadUp)) {
        if (m_menu_current_option > 0) {
            m_menu_current_option--;
        } else {
            m_menu_current_option = m_menu_options_count - 1;
        }
        refresh_menu();
        return Command::None;
    } else if (is_only_holding(Key::DpadDown)) {
        if (m_menu_current_option < m_menu_options_count - 1) {
            m_menu_current_option++;
        } else {
            m_menu_current_option = 0;
        }
        refresh_menu();
        return Command::None;
    }
    switch (m_mode) {
    case Mode::Active:
        return Command::None;
    case Mode::SettingHome:
        if (is_only_holding(Key::B)) {
            m_mode = Mode::Active;
            clear_hold();
            return Command::SwitchMode;
        }
        if (is_only_holding(Key::A)) {
            switch (m_menu_current_option) {
            case 0:
                m_mode = Mode::SettingKeyBinding;
                m_menu_current_option = 0;
                break;
            case 1:
                m_mode = Mode::SettingStateOption;
                m_menu_current_option = 0;
                break;
            }
            refresh_menu();
            return Command::None;
        }
        break;
    case Mode::SettingKeyBinding:
        if (is_configuring_key()) {
            if (is_only_holding(Key::ZL)) {
                if (has_held_keys_for(Key::ZL, 3)) {
                    m_key_being_configured = nullptr;
                    m_tick_since_last_menu_input = 0;
                    refresh_menu();
                    return Command::None;
                }
            }
            Key new_key = get_hold_keys();
            if (finish_configure_key(new_key)) {
                    mem::StringBuffer<16> key_name;
                get_key_name(key_name, m_key_being_configured);
                    mem::StringBuffer<120> buffer;
                get_key_string(new_key, buffer);
                msg::widget::printf("Changed %s key\n to: %s", key_name.content(), buffer.content());

                m_key_being_configured = nullptr;
                m_tick_since_last_menu_input = 0;
                m_menu_showing_explain_message = true;
                return Command::SaveOption;
            }
            return Command::None;
        }
        if (is_only_holding(Key::B)) {
            m_mode = Mode::SettingHome;
            m_menu_current_option = 0;
            refresh_menu();
            return Command::None;
        }
        if (is_only_holding(Key::A)) {
            switch (m_menu_current_option) {
            case 0:
                start_configure_key(&m_key_save);
                break;
            case 1:
                start_configure_key(&m_key_restore);
                break;
            case 2:
                start_configure_key(&m_key_save_file);
                break;
            case 3:
                start_configure_key(&m_key_restore_file);
                break;
            }
            return Command::None;
        }
        break;
    case Mode::SettingStateOption:
        if (is_only_holding(Key::B)) {
            m_mode = Mode::SettingHome;
            m_menu_current_option = 1;
            refresh_menu();
            return Command::None;
        }
        if (is_only_holding(Key::X)) {
            m_menu_showing_explain_message = true;
            m_tick_since_last_menu_input = 0;
            switch (m_menu_current_option) {
            case 0:
                msg::widget::print(
                    "Restore Message\n\nWhen disabled, there will be no\nmessage shown when "
                    "executing a\nstate restore. Useful for grinding\nBTT segments.");
                break;
            case 1:
                msg::widget::print("Timers\n\nEnable save/restore TOD, Bloodmoon,\nChampion Ability, "
                                 "Master Sword,\nPotion, and Weather Damage Timers.");
                break;
            case 2:
                msg::widget::print("Inventory\n\nEnable save/restore the inventory\nstate. Equipments will be synced.");
                break;
            }
        }
        if (is_only_holding(Key::A)) {
            bool value;
            switch (m_menu_current_option) {
            case 0:
                value = !m_config->m_show_restore_message;
                msg::info::printf("Restore Message: %s", get_enabled_text(value));
                m_config->m_show_restore_message = value;
                break;
            case 1:
                value = !m_config->m_enable_timers;
                msg::info::printf("Timers: %s", get_enabled_text(value));
                m_config->m_enable_timers = value;
                break;
            case 2:
                value = !m_config->m_enable_inventory;
                msg::info::printf("Inventory: %s", get_enabled_text(value));
                m_config->m_enable_inventory = value;
                break;
            }
            refresh_menu();
            return Command::SaveOption;
        }
        break;
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

void Controller::refresh_menu() {
    m_menu_showing_explain_message = false;
    m_tick_since_last_menu_input = 0;
    switch (m_mode) {
    case Mode::Active:
        return;
    case Mode::SettingHome:
        m_menu_title.copy("Save State Settings");
        m_menu_subtitle.copy("What do you want to do?");
        m_menu_options_count = 2;
        m_menu_options[0].copy("Change Key Binding");
        m_menu_options[1].copy("Change State Options");
        break;
    case Mode::SettingKeyBinding:
        if (is_configuring_key()) {
            show_configuring_key_message();
            return;
        }
        m_menu_title.copy("Change Key Binding");
        m_menu_subtitle.copy("Which key to change?");
        m_menu_options_count = 4;
        m_menu_options[0].copy("Save To Mem");
        m_menu_options[1].copy("Restore From Mem");
        m_menu_options[2].copy("Save To File");
        m_menu_options[3].copy("Restore From File");
        break;
    case Mode::SettingStateOption:
        m_menu_title.copy("Change State Options");
        m_menu_subtitle.copy("[A] Toggle; [X] Explain");
        m_menu_options_count = 3;
        m_menu_options[0].clear();
        m_menu_options[0].appendf("[%s] Restore Message",
                                  get_toggle_text(m_config->m_show_restore_message));
        m_menu_options[1].clear();
        m_menu_options[1].appendf("[%s] Timers", get_toggle_text(m_config->m_enable_timers));
        m_menu_options[2].clear();
        m_menu_options[2].appendf("[%s] Inventory", get_toggle_text(m_config->m_enable_inventory));
        break;
    }
    if (m_menu_current_option >= m_menu_options_count) {
        m_menu_current_option = 0;
    }
    mem::StringBuffer<MENU_BUFFER_LEN * 7> buffer;
    buffer.appendf("%s\n%s", m_menu_title.content(), m_menu_subtitle.content());
    for (u32 i = 0; i < m_menu_options_count; i++) {
        buffer.append("\n");
        if (m_menu_current_option == i) {
            buffer.appendf("-> ");
        } else {
            buffer.appendf("      ");
        }
        buffer.appendf("%s", m_menu_options[i].content());
    }
    msg::widget::print(buffer.content());
}

void Controller::show_configuring_key_message() {
    if (!m_key_being_configured) {
        return;
    }
    mem::StringBuffer<16> key_name;
    get_key_name(key_name, m_key_being_configured);
    mem::StringBuffer<50> current_combo;
    get_key_string(*m_key_being_configured, current_combo);
    msg::widget::printf(
        "Changing: %s\nCurrent:\n%s\nHold new combo for 3 seconds.\nHold ZL to cancel",
        key_name.content(), current_combo.content());
}

}  // namespace botw::savs

#pragma once

#include <exl/types.h>
#include <toolkit/io/data_reader.hpp>
#include <toolkit/io/data_writer.hpp>
#include <toolkit/mem/string.hpp>

#include "core/state.hpp"

namespace sead {
struct Controller;
}

namespace botw::savs {
enum Key : u32 {
    None = 0,
    A = 1,
    B = 1 << 1,
    ZL = 1 << 2,
    X = 1 << 3,
    Y = 1 << 4,
    ZR = 1 << 5,
    RStick = 1 << 6,
    LStick = 1 << 7,

    Minus = (1 << 9) | (1 << 12),
    Plus = (1 << 10) | (1 << 11),

    L = 1 << 13,
    R = 1 << 14,

    DpadUp = 1 << 16,
    DpadDown = 1 << 17,
    DpadLeft = 1 << 18,
    DpadRight = 1 << 19

};

inline constexpr Key operator|(Key a, Key b) {
    return static_cast<Key>(static_cast<u32>(a) | static_cast<u32>(b));
}

constexpr Key KEY_SETTINGS =
    Key::DpadDown | Key::ZL | Key::ZR | Key::L | Key::R;
constexpr Key KEY_INCREASE_LEVEL = Key::R;
constexpr Key KEY_DECREASE_LEVEL = Key::L;

template <u32 L>
void get_key_string(u32 keys, mem::StringBuffer<L>& out_buffer) {
    out_buffer.clear();
    if (keys == 0) {
        return;
    }
    if (keys & Key::ZL) {
        out_buffer.append("ZL+");
    }
    if (keys & Key::ZR) {
        out_buffer.append("ZR+");
    }
    if (keys & Key::L) {
        out_buffer.append("L+");
    }
    if (keys & Key::R) {
        out_buffer.append("R+");
    }
    if (keys & Key::A) {
        out_buffer.append("A+");
    }
    if (keys & Key::B) {
        out_buffer.append("B+");
    }
    if (keys & Key::X) {
        out_buffer.append("X+");
    }
    if (keys & Key::Y) {
        out_buffer.append("Y+");
    }
    if (keys & Key::LStick) {
        out_buffer.append("L3+");
    }
    if (keys & Key::RStick) {
        out_buffer.append("R3+");
    }
    if (keys & Key::DpadUp) {
        out_buffer.append("DpadUp+");
    }
    if (keys & Key::DpadDown) {
        out_buffer.append("DpadDown+");
    }
    if (keys & Key::DpadLeft) {
        out_buffer.append("DpadLeft+");
    }
    if (keys & Key::DpadRight) {
        out_buffer.append("DpadRight+");
    }
    if (keys & Key::Minus) {
        out_buffer.append("Minus+");
    }
    if (keys & Key::Plus) {
        out_buffer.append("Plus+");
    }
    out_buffer.delete_end(1);
}

enum class Command {
    None,
    Save,
    SaveFile,
    Restore,
    RestoreFile,
    PostRestoreHold,
    RestoreDone,

    SwitchMode,
    SaveOption,
};

constexpr size_t MENU_BUFFER_LEN = 40;
constexpr u32 MENU_REFRESH_TICKS =
    60; // refresh menu after this time if there is no input
constexpr u32 MENU_INPUT_INTERVAL = 3;

class Controller {
public:
    enum class Mode : u32 {
        Active,
        SettingHome,
        SettingKeyBinding,
        SettingStateOption,
        SettingOtherFunction,
    };

    bool initialize();
    Command update();

    void save_key_bindings(io::DataWriter& writer) const;
    void load_key_bindings(io::DataReader& reader);

    Mode get_mode() const { return m_mode; }
    bool is_in_settings() const { return m_mode != Mode::Active; }

public:
    StateConfig* m_config = nullptr;

private:
    sead::Controller* m_controller = nullptr;

    Mode m_mode = Mode::Active;
    Key m_holding_keys = Key::None;
    u32 m_hold_counter = 0;
    Key* m_key_being_configured = nullptr;
    u32 m_configure_hold_counter = 0;
    Key m_key_save = Key::ZL | Key::L | Key::Plus | Key::DpadLeft;
    Key m_key_save_file = Key::ZL | Key::L | Key::DpadLeft | Key::RStick;
    Key m_key_restore = Key::ZL | Key::L | Key::Plus | Key::DpadRight;
    Key m_key_restore_file = Key::ZL | Key::L | Key::DpadRight | Key::RStick;
    bool m_restore_fired = false;

    u32 m_tick_since_last_menu_input = MENU_REFRESH_TICKS + 1;
    mem::StringBuffer<MENU_BUFFER_LEN> m_menu_title;
    mem::StringBuffer<MENU_BUFFER_LEN> m_menu_subtitle;
    mem::StringBuffer<MENU_BUFFER_LEN> m_menu_options[4];
    u32 m_menu_options_count = 0;
    u32 m_menu_current_option = 0;
    bool m_menu_showing_explain_message = false;

    bool is_only_holding(Key keys) const;
    Key get_hold_keys() const;
    bool has_held_keys_for(Key keys, u32 seconds);
    void clear_hold() {
        m_holding_keys = Key::None;
        m_hold_counter = 0;
    }
    bool is_configuring_key() const {
        return m_key_being_configured != nullptr;
    }
    void start_configure_key(Key* key);
    bool finish_configure_key(Key new_key);
    void get_key_name(mem::StringBuffer<16>& out_buffer, Key* key) const {
        out_buffer.clear();
        if (key == &m_key_save) {
            out_buffer.append("SaveToMem");
        } else if (key == &m_key_save_file) {
            out_buffer.append("SaveToFile");
        } else if (key == &m_key_restore) {
            out_buffer.append("RestoreMem");
        } else if (key == &m_key_restore_file) {
            out_buffer.append("RestoreFile");
        }
    }

    Command update_active_mode();
    Command update_setting_mode();
    void refresh_menu();
    void show_configuring_key_message();
};
} // namespace botw::savs

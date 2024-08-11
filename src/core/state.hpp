#pragma once

#include <exl/types.h>
#include "core/reporter.hpp"
#include "util/data_reader.hpp"
#include "util/data_writer.hpp"

namespace botw::savs {

enum class StateFileResult {
    Ok,
    UnsupportedVersion,
    InvalidVersion,
    IOError,
};

class StateConfig {
public:
    void save_config(DataWriter& w) const;
    void read_config(DataReader& r);
    // show info message when restoring
    bool m_show_restore_message = true;
    // enable TOD, bloodmoon, ability, master sword, potion, and climate damage timers
    bool m_enable_timers = true;
    // enable restoring overworld equipment durability, if the same equipment is equipped
    bool m_enable_overworld_durability = false;
    // enable restoring inventory state
    bool m_enable_inventory = false;
};

class State {
public:
    void read_from_game(Reporter& reporter, const StateConfig& config);
    void write_to_game(Reporter& reporter, const StateConfig& config, bool hold) const;
    StateFileResult read_from_file(DataReader& reader);
    StateFileResult write_to_file(DataWriter& writer) const;

    bool is_stored() const {
        return m_stored_essentials || m_stored_timers || m_stored_overworld_durability ||
               m_stored_inventory;
    }
    void clear() {
        m_stored_essentials = false;
        m_stored_timers = false;
        m_stored_overworld_durability = false;
        m_stored_inventory = false;
    }

private:
    // config
    bool m_stored_essentials = false;
    bool m_stored_timers = false;
    bool m_stored_overworld_durability = false;
    bool m_stored_inventory = false;
    // always enabled
    u32 m_health;
    float m_stamina;
    float m_havok_position[3];
    float m_main_position_matrix[12];
    float m_camera_pan_matrix[12];
    float m_camera_zoom;
    float m_camera_tilt;
    /* Extra stuff always restored:
     * - Rune cooldown
     * - Delete bombs
     * - TODO: fall damage
     * - TODO: selected rune
     *
     */

    // timers
    float m_tod_paused;
    float m_tod_unpaused;
    float m_blood_moon_timer;
    float m_temperature_damage_timer;
    float m_flame_timer;
    float m_gale_timer;
    float m_fury_timer;
    float m_protection_timer;
    float m_grace_timer;
    u32 m_ability_uses[3];
    float m_master_sword_cooldown;
    float m_speed_potion_timer_1;
    float m_speed_potion_timer_2;
    float m_speed_potion_timer_3;
    float m_attack_potion_timer;
    float m_defense_potion_timer;
    float m_heat_resist_potion_timer;
    float m_cold_resist_potion_timer;
    float m_flame_resist_potion_timer;
    float m_shock_resist_potion_timer;
    float m_stealth_potion_timer;

    // durability
    NamedValue<u32, 64> m_overworld_equipped_weapon_durability{0};
    NamedValue<u32, 64> m_overworld_equipped_bow_durability{0};
    NamedValue<u32, 64> m_overworld_equipped_shield_durability{0};

    // inventory (TODO - save and restore entire PMDM
    NamedValue<u32, 64> m_menu_equipped_arrow_count{0};
    NamedValue<u32, 64> m_menu_equipped_weapon_durability{0};
    NamedValue<u32, 64> m_menu_equipped_bow_durability{0};
    NamedValue<u32, 64> m_menu_equipped_shield_durability{0};
    s32 m_num_inventory_count_offset;
};

}  // namespace botw::savs

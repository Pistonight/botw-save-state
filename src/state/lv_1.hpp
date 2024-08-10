#pragma once

#include <exl/types.h>
#include "./version.hpp"

namespace botwsavs::util {
class DataReader;
class DataWriter;
}


namespace botwsavs::state {

class Reporter;

class Lv1 {
public:
    void read_from_game(Reporter& reporter);
    void write_to_game(Reporter& reporter, bool hold) const;
    void read_from_file(util::DataReader& reader, Version version);
    void write_to_file(util::DataWriter& writer) const;

private:
    u32 m_health;
    float m_stamina;
    float m_havok_position[3];
    float m_main_position_matrix[12];
    float m_camera_pan_matrix[12];
    float m_camera_zoom;
    float m_camera_tilt;
    // Extras:
    // reset rune cooldown
    // TODO reset fall damage

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
};

}  // namespace botwsavs::core

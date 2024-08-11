/**
 * Credit to Bloom (Swiffy22) for finding the pointers
 *
 * Some pointers are not used. See data/raw_ptr.txt for the original list
 * from the research.
 */
#pragma once
#include <exl/types.h>
#include "util/safe_memory.hpp"

extern "C" void* main_memory;

#define _main (mem_ptr(&main_memory))

namespace ksys::act {
struct BaseProc;
}

namespace uking::ui {
struct PouchItem;
}

namespace botw::savs::raw_ptr {
// Health and Stam
inline safe_ptr<u32> health() {
    return _main[0x2CA1A78][0x80] + 0x848;
}
inline safe_ptr<float> stamina() {
    return _main[0x2c9fd70][0x38] + 0x2ac;
}
// Max Stam Amount (unused):	[[main+2c9fd70]+38]+2b0

// Runes
// Set Bomb cool down to 360 in float (43350000)
// Set Stasis cool down to 1 in float (3F800000)
inline safe_ptr<float> round_bomb_cooldown() {
    return _main[0x2CA3AD8][0x30] + 0x80;
}
inline safe_ptr<float> square_bomb_cooldown() {
    return _main[0x2CA3AD8][0x30] + 0x98;
}
inline safe_ptr<float> stasis_cooldown() {
    return _main[0x2CA3AD8][0x30] + 0x144;
}

// Position and Camera
inline safe_ptr<float> havok_position() {
    return _main[0x2D37AF0][0xF0][0xD28] + 0x5E0;
}
inline safe_ptr<float> main_position_matrix() {
    return _main[0x2CA1140][0x60] + 0x398;
}
inline safe_ptr<float> camera_pan_matrix() {
    return _main[0x2CE1090][0x60] + 0x20;
}
inline safe_ptr<float> camera_zoom() {
    return _main[0x2CA5520][0x20][0x558] + 0xD48;
}
inline safe_ptr<float> camera_tilt() {
    return _main[0x2CA5520][0x20][0x558] + 0xD14;
}

// Fall Damage (need to figure out how to make FDC work)
inline safe_ptr<float> fall_damage_y() {
    return _main[0x2CA1140][0x60] + 0x20D0;
}

// Overworld Durability
inline safe_ptr<ksys::act::BaseProc> overworld_weapon_actor() {
    return _main[0x2CC4768][0xc0][-0xA0][0x48];
}

inline safe_ptr<ksys::act::BaseProc> overworld_bow_actor() {
    return _main[0x2CC4768][0xc0][-0x70][0x48];
}

inline safe_ptr<ksys::act::BaseProc> overworld_shield_actor() {
    return _main[0x2CC4768][0xc0][-0x88][0x48];
}

inline safe_ptr<u32> overworld_weapon_durability() {
    return _main[0x2CC4768][0xc0][-0xA0][0x48] + 0xD4C;
}

inline safe_ptr<u32> overworld_bow_durability() {
    return _main[0x2CC4768][0xc0][-0x70][0x48] + 0xD4C;
}

inline safe_ptr<u32> overworld_shield_durability() {
    return _main[0x2CC4768][0xc0][-0x88][0x48] + 0xD4C;
}

// Time
inline safe_ptr<float> time_of_day_paused() {
    return _main[0x2CC5FE0][0xBE0][0x30][0xF8] + 0x18;
}
inline safe_ptr<float> time_of_day_unpaused() {
    return _main[0x2CD18C8][0x17630] + 0x468;
}
inline safe_ptr<float> blood_moon() {
    return _main[0x2CD18D0][0x30][0x398] + 0x2D0;
}

// Climate Damage
inline safe_ptr<float> temperature_damage_timer() {
    return _main[0x2CA1140][0x60] + 0x268C;
}
inline safe_ptr<float> flame_timer() {
    return _main[0x2CA3A68][0x1438][0xB0] + 0x258;
}

// Champion Abilities
inline safe_ptr<float> gale_timer() {
    return _main[0x2CA1140][0x60] + 0x1DF4;
}
inline safe_ptr<float> fury_timer() {
    return _main[0x2CA1140][0x60] + 0x1E00;
}
inline safe_ptr<float> protection_timer() {
    return _main[0x2CA1140][0x60] + 0x1E0C;
}
inline safe_ptr<float> grace_timer() {
    return _main[0x2CA1140][0x60] + 0x1E18;
}
inline safe_ptr<u32> ability_uses() {
    return _main[0x2CA1A78][0x80] + 0x1CC8;
}

// Master Sword
inline safe_ptr<float> master_sword_cooldown() {
    return _main[0x2CA1140][0x60] + 0x1E24;
}

// Potion
inline safe_ptr<float> speed_potion_timer1() {
    return _main[0x2CC4768][0xC0] + 0x12A0;
}
inline safe_ptr<float> speed_potion_timer2() {
    return _main[0x2CC4768][0xC0] + 0x12AC;
}
inline safe_ptr<float> speed_potion_timer3() {
    return _main[0x2CC4768][0xC0] + 0x12B8;
}
inline safe_ptr<float> attack_potion_timer() {
    return _main[0x2CC4768][0xC0] + 0x12DC;
}
inline safe_ptr<float> defense_potion_timer() {
    return _main[0x2CC4768][0xC0] + 0x12E8;
}
inline safe_ptr<float> heat_resist_potion_timer() {
    return _main[0x2CC4768][0xC0] + 0x12F4;
}
inline safe_ptr<float> cold_resist_potion_timer() {
    return _main[0x2CC4768][0xC0] + 0x1300;
}
inline safe_ptr<float> flame_resist_potion_timer() {
    return _main[0x2CC4768][0xC0] + 0x130C;
}
inline safe_ptr<float> shock_resist_potion_timer() {
    return _main[0x2CC4768][0xC0] + 0x1324;
}
inline safe_ptr<float> stealth_potion_timer() {
    return _main[0x2CC4768][0xC0] + 0x133C;
}

}  // namespace botw::savs::raw_ptr
#undef _main

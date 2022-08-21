#pragma once
#include "MemPtr.hpp"

#define main (MemPtr())
#define DefinePtr(TYPE, NAME, PATH) inline util::SafePtr<TYPE> NAME() { return (PATH).AsPtrTo<TYPE>(); } 

namespace botwsavs::core::ptr {

// Health and Stam
DefinePtr(u32, Health, main[0x2CA1A78][0x80] + 0x848) // [[main+2CA1A78]+80]+848
DefinePtr(f32, Stamina, main[0x2c9fd70][0x38] + 0x2ac) // [[main+2c9fd70]+38]+2ac
// Max Stam Amount (unused):	[[main+2c9fd70]+38]+2b0

// Runes
// Set Bomb cool down to 360 in float (43350000)
// Set Stasis cool down to 1 in float (3F800000)
DefinePtr(f32, RoundBombCooldown, main[0x2CA3AD8][0x30] + 0x80) // [[main+2CA3AD8]+30]+80
DefinePtr(f32, SquareBombCooldown, main[0x2CA3AD8][0x30] + 0x98) // [[main+2CA3AD8]+30]+98
DefinePtr(f32, StasisCooldown, main[0x2CA3AD8][0x30] + 0x144) // [[main+2CA3AD8]+30]+144

// Position and Camera
DefinePtr(f32, HavokPosition, main[0x2D37AF0][0xF0][0xD28] + 0x5E0) // [[[main+2D37AF0]+F0]+D28]+5E0 length=3 not 100% stable
DefinePtr(f32, MainPositionMatrix, main[0x2CA1140][0x60] + 0x398) // [[main+2CA1140]+60]+398 length=12
DefinePtr(f32, CameraPanMatrix, main[0x2CE1090][0x60] + 0x20) // [[main+2CE1090]+60]+20
DefinePtr(f32, CameraZoom, main[0x2CA5520][0x20][0x558] + 0xD48) // [[[main+2CA5520]+20]+558]+D48 
DefinePtr(f32, CameraTilt, main[0x2CA5520][0x20][0x558] + 0xD14) // [[[main+2CA5520]+20]+558]+D14

// Fall Damage (need to figure out how to make FDC work)
DefinePtr(f32, FallDamageY, main[0x2CA1140][0x60] + 0x20D0) // [[main+2CA1140]+60]+20D0

// Durability
DefinePtr(u32, MenuEquippedArrowCount, main[0x2CA6D48][0x447E8] + 0x20) // [[main+2CA6D48]+447E8]+20 only works when in menu
DefinePtr(f32, OverworldWeaponDurability, main[0x2CC4768][0xc0][-0xA0][0x48] + 0xD4c) // [[[[main+2CC4768]+c0]-A0]+48]+D4C
DefinePtr(f32, OverworldBowDurability, main[0x2CC4768][0xc0][-0x70][0x48] + 0xD4c) // [[[[main+2CC4768]+c0]-70]+48]+D4C
DefinePtr(f32, OverworldShieldDurability, main[0x2CC4768][0xc0][-0x88][0x48] + 0xD4c) // [[[[main+2CC4768]+c0]-88]+48]+D4C
// Inventory Weapon: 			[[main+2CA6D48]+447D8]+20
// Inventory Bow: 				[[main+2ca6d48]+447E0]+20
// Inventory Shield:			[[main+2ca6d48]+447F0]+20
// Don't use Inventory equipment. Requires restoring in menu + unequip and re-equip to work. Just use overworld instance.

// Time
DefinePtr(f32, TimeOfDayPaused, main[0x2CC5FE0][0xBE0][0x30][0xF8] + 0x18) // [[[[main+2CC5FE0]+BE0]+30]+F8]+18
DefinePtr(f32, TimeOfDayUnpaused, main[0x2CD18C8][0x17630] + 0x468) // [[main+2CD18C8]+17630]+468
DefinePtr(f32, BloodMoonTimer, main[0x2CD18D0][0x39][0x398] + 0x2D0) // [[[main+2CD18D0]+30]+398]+2D0

// Climate Damage
DefinePtr(f32, TemperatureDamageTimer, main[0x2CA1140][0x60] + 0x268C) // [[main+2CA1140]+60]+268C
DefinePtr(f32, FlameTimer, main[0x2CA3A68][0x1438][0xB0] + 0x258) // [[[main+2CA3A68]+1438]+B0]+258

// Champion Abilities
DefinePtr(f32, GaleTimer, main[0x2CA1140][0x60] + 0x1DF4) // [[main+2CA1140]+60]+1DF4
DefinePtr(f32, FuryTimer, main[0x2CA1140][0x60] + 0x1E00) // [[main+2CA1140]+60]+1E00
DefinePtr(f32, ProtectionTimer, main[0x2CA1140][0x60] + 0x1E0C) // [[main+2CA1140]+60]+1E0C
DefinePtr(f32, GraceTimer, main[0x2CA1140][0x60] + 0x1E18) // [[main+2CA1140]+60]+1E18
DefinePtr(u32, AbilityUses, main[0x2CA1A78][0x80] + 0x1CC8) // [[main+2ca1a78]+80]+1CC8 length = 3

// Master Sword
DefinePtr(f32, MasterSwordCooldown, main[0x2CA1140][0x60] + 0x1E24) // [[main+2CA1140]+60]+1E24

// Potion
DefinePtr(f32, SpeedPotionTimer1, main[0x2CC4768][0xC0] + 0x12A0) // [[main+2CC4768]+C0]+12A0
DefinePtr(f32, SpeedPotionTimer2, main[0x2CC4768][0xC0] + 0x12AC) // [[main+2CC4768]+C0]+12AC
DefinePtr(f32, SpeedPotionTimer3, main[0x2CC4768][0xC0] + 0x12B8) // [[main+2CC4768]+C0]+12B8
DefinePtr(f32, AttackPotionTimer, main[0x2CC4768][0xC0] + 0x12DC) // [[main+2CC4768]+C0]+12DC
DefinePtr(f32, DefensePotionTimer, main[0x2CC4768][0xC0] + 0x12E8) // [[main+2CC4768]+C0]+12E8
DefinePtr(f32, HeatResistPotionTimer, main[0x2CC4768][0xC0] + 0x12F4) // [[main+2CC4768]+C0]+12F4
DefinePtr(f32, ColdResistPotionTimer, main[0x2CC4768][0xC0] + 0x1300) // [[main+2CC4768]+C0]+1300
DefinePtr(f32, FlameResistPotionTimer, main[0x2CC4768][0xC0] + 0x130C) // [[main+2CC4768]+C0]+130C
DefinePtr(f32, ShockResistPotinoTimer, main[0x2CC4768][0xC0] + 0x1324) // [[main+2CC4768]+C0]+1324
DefinePtr(f32, StealthPotinoTimer, main[0x2CC4768][0xC0] + 0x133C) // [[main+2CC4768]+C0]+133C

}  // namespace botwsavs::core::ptr

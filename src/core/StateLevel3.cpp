#include "State.hpp"
#include "StateMacros.hpp"
#include "fs/Logger.hpp"
#include "mem/GamePtr.h"

namespace botwsavs::core {

void State::ReadLevel3() {
    GameReadEz(TimeOfDayPaused);
    GameReadEz(TimeOfDayUnpaused);
    GameReadEz(BloodMoonTimer);
    GameReadEz(TemperatureDamageTimer);
    GameReadEz(FlameTimer);
    GameReadEz(GaleTimer);
    GameReadEz(FuryTimer);
    GameReadEz(ProtectionTimer);
    GameReadEz(GraceTimer);
    GameReadArrayEz(AbilityUses, 3);
    GameReadEz(MasterSwordCooldown);
    GameReadEz(SpeedPotionTimer1);
    GameReadEz(SpeedPotionTimer2);
    GameReadEz(SpeedPotionTimer3);
    GameReadEz(AttackPotionTimer);
    GameReadEz(DefensePotionTimer);
    GameReadEz(HeatResistPotionTimer);
    GameReadEz(ColdResistPotionTimer);
    GameReadEz(FlameResistPotionTimer);
    GameReadEz(ShockResistPotionTimer);
    GameReadEz(StealthPotionTimer);
}

void State::WriteLevel3() {
    GameWriteEz(TimeOfDayPaused);
    GameWriteEz(TimeOfDayUnpaused);
    GameWriteEz(BloodMoonTimer);
    GameWriteEz(TemperatureDamageTimer);
    GameWriteEz(FlameTimer);
    GameWriteEz(GaleTimer);
    GameWriteEz(FuryTimer);
    GameWriteEz(ProtectionTimer);
    GameWriteEz(GraceTimer);
    GameWriteArrayEz(AbilityUses, 3);
    GameWriteEz(MasterSwordCooldown);
    GameWriteEz(SpeedPotionTimer1);
    GameWriteEz(SpeedPotionTimer2);
    GameWriteEz(SpeedPotionTimer3);
    GameWriteEz(AttackPotionTimer);
    GameWriteEz(DefensePotionTimer);
    GameWriteEz(HeatResistPotionTimer);
    GameWriteEz(ColdResistPotionTimer);
    GameWriteEz(FlameResistPotionTimer);
    GameWriteEz(ShockResistPotionTimer);
    GameWriteEz(StealthPotionTimer);
}

}  // namespace botwsavs::core

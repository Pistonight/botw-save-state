#pragma once

#include <exl/types.h>
#include "state/version.hpp"

namespace botwsavs::util {
class DataReader;
class DataWriter;
}

namespace botwsavs::state {

class Reporter;

class Lv3 {
public:
    void read_from_game(Reporter& reporter);
    void write_to_game(Reporter& reporter, bool hold) const;
    void read_from_file(util::DataReader& reader, Version version);
    void write_to_file(util::DataWriter& writer) const;

private:
    float mTimeOfDayPaused;
    float mTimeOfDayUnpaused;
    float mBloodMoonTimer;
    float mTemperatureDamageTimer;
    float mFlameTimer;
    float mGaleTimer;
    float mFuryTimer;
    float mProtectionTimer;
    float mGraceTimer;
    u32 mAbilityUses[3];
    float mMasterSwordCooldown;

    float mSpeedPotionTimer1;
    float mSpeedPotionTimer2;
    float mSpeedPotionTimer3;
    float mAttackPotionTimer;
    float mDefensePotionTimer;
    float mHeatResistPotionTimer;
    float mColdResistPotionTimer;
    float mFlameResistPotionTimer;
    float mShockResistPotionTimer;
    float mStealthPotionTimer;

    /* s32 mCountOffset; */
};
}  // namespace botwsavs::core

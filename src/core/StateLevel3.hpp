#pragma once

#include "StateStorage.hpp"

namespace botwsavs::core {

class StateLevel3 : public StateStorage {
public:
    void ReadFromGame() override;
    void WriteToGame() override;
    void ReadFromFile(fs::ConfigFile& file, u32 version) override;
    void WriteToFile(fs::ConfigFile& file) const override;

private:
    f32 mTimeOfDayPaused;
    f32 mTimeOfDayUnpaused;
    f32 mBloodMoonTimer;
    f32 mTemperatureDamageTimer;
    f32 mFlameTimer;
    f32 mGaleTimer;
    f32 mFuryTimer;
    f32 mProtectionTimer;
    f32 mGraceTimer;
    u32 mAbilityUses[3];
    f32 mMasterSwordCooldown;

    f32 mSpeedPotionTimer1;
    f32 mSpeedPotionTimer2;
    f32 mSpeedPotionTimer3;
    f32 mAttackPotionTimer;
    f32 mDefensePotionTimer;
    f32 mHeatResistPotionTimer;
    f32 mColdResistPotionTimer;
    f32 mFlameResistPotionTimer;
    f32 mShockResistPotionTimer;
    f32 mStealthPotionTimer;

    s32 mCountOffset;
};
}  // namespace botwsavs::core

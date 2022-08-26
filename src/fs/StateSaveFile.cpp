#include "StateSaveFile.hpp"
#include "core/State.hpp"
#include "fs/Logger.hpp"

namespace botwsavs::fs {
void StateSaveFile::LoadInternal() {
    u32 version = 0;
    ReadInteger(&version);

    switch (version) {
    case 1:
        ReadVersion1();
        break;
    case 2:
        ReadVersion2();
        break;
    case 3:
        ReadVersion3();
        break;
    default:
        errorf("Bad version: %d", version);
    }
}

void StateSaveFile::SaveInternal() {
    WriteInteger("version", STATE_SAVE_FILE_VERSION);
    StateFileWrite(Integer, mLevel);
    // Level 1
    StateFileWrite(Integer, mHealth);
    StateFileWrite(Float, mStamina);
    StateFileWriteArray(Float, mHavokPosition, 3);
    StateFileWriteArray(Float, mMainPositionMatrix, 12);
    StateFileWriteArray(Float, mCameraPanMatrix, 12);
    StateFileWrite(Float, mCameraZoom);
    StateFileWrite(Float, mCameraTilt);
    // Level 2
    StateFileWriteNamedValue(Integer, mMenuEquippedArrow);
    StateFileWriteNamedValue(Integer, mMenuEquippedWeapon);
    StateFileWriteNamedValue(Integer, mMenuEquippedBow);
    StateFileWriteNamedValue(Integer, mMenuEquippedShield);
    StateFileWriteNamedValue(Integer, mOverworldEquippedWeapon);
    StateFileWriteNamedValue(Integer, mOverworldEquippedBow);
    StateFileWriteNamedValue(Integer, mOverworldEquippedShield);
    // Level 3
    StateFileWrite(Float, mTimeOfDayPaused);
    StateFileWrite(Float, mTimeOfDayUnpaused);
    StateFileWrite(Float, mBloodMoonTimer);
    StateFileWrite(Float, mTemperatureDamageTimer);
    StateFileWrite(Float, mFlameTimer);
    StateFileWrite(Float, mGaleTimer);
    StateFileWrite(Float, mFuryTimer);
    StateFileWrite(Float, mProtectionTimer);
    StateFileWrite(Float, mGraceTimer);
    StateFileWriteArray(Integer, mAbilityUses, 3);
    StateFileWrite(Float, mMasterSwordCooldown);
    StateFileWrite(Float, mSpeedPotionTimer1);
    StateFileWrite(Float, mSpeedPotionTimer2);
    StateFileWrite(Float, mSpeedPotionTimer3);
    StateFileWrite(Float, mAttackPotionTimer);
    StateFileWrite(Float, mDefensePotionTimer);
    StateFileWrite(Float, mHeatResistPotionTimer);
    StateFileWrite(Float, mColdResistPotionTimer);
    StateFileWrite(Float, mFlameResistPotionTimer);
    StateFileWrite(Float, mShockResistPotionTimer);
    StateFileWrite(Float, mStealthPotionTimer);
}

void StateSaveFile::ReadVersion1() {
    StateFileRead(Integer, mLevel);
    StateFileRead(Integer, mHealth);
    StateFileRead(Float, mStamina);
    StateFileReadArray(Float, mHavokPosition, 3);
    StateFileReadArray(Float, mMainPositionMatrix, 12);
    StateFileReadArray(Float, mCameraPanMatrix, 12);
    StateFileRead(Float, mCameraZoom);
    StateFileRead(Float, mCameraTilt);
}

void StateSaveFile::ReadVersion2() {
    ReadVersion1();
    StateFileReadNamedValue(Integer, mMenuEquippedArrow, 64);
    StateFileReadNamedValue(Integer, mMenuEquippedWeapon, 64);
    StateFileReadNamedValue(Integer, mMenuEquippedBow, 64);
    StateFileReadNamedValue(Integer, mMenuEquippedShield, 64);
    StateFileReadNamedValue(Integer, mOverworldEquippedWeapon, 64);
    StateFileReadNamedValue(Integer, mOverworldEquippedBow, 64);
    StateFileReadNamedValue(Integer, mOverworldEquippedShield, 64);
}

void StateSaveFile::ReadVersion3() {
    ReadVersion2();
    StateFileRead(Float, mTimeOfDayPaused);
    StateFileRead(Float, mTimeOfDayUnpaused);
    StateFileRead(Float, mBloodMoonTimer);
    StateFileRead(Float, mTimeOfDayPaused);
    StateFileRead(Float, mTimeOfDayUnpaused);
    StateFileRead(Float, mBloodMoonTimer);
    StateFileRead(Float, mTemperatureDamageTimer);
    StateFileRead(Float, mFlameTimer);
    StateFileRead(Float, mGaleTimer);
    StateFileRead(Float, mFuryTimer);
    StateFileRead(Float, mProtectionTimer);
    StateFileRead(Float, mGraceTimer);
    StateFileReadArray(Integer, mAbilityUses, 3);
    StateFileRead(Float, mMasterSwordCooldown);
    StateFileRead(Float, mSpeedPotionTimer1);
    StateFileRead(Float, mSpeedPotionTimer2);
    StateFileRead(Float, mSpeedPotionTimer3);
    StateFileRead(Float, mAttackPotionTimer);
    StateFileRead(Float, mDefensePotionTimer);
    StateFileRead(Float, mHeatResistPotionTimer);
    StateFileRead(Float, mColdResistPotionTimer);
    StateFileRead(Float, mFlameResistPotionTimer);
    StateFileRead(Float, mShockResistPotionTimer);
    StateFileRead(Float, mStealthPotionTimer);
}
}  // namespace botwsavs::fs

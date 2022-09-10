#include <Game/UI/uiPauseMenuDataMgr.h>
#include <container/seadOffsetList.h>

#include "fs/ConfigFile.hpp"
#include "fs/Logger.hpp"
#include "mem/GamePtr.h"

#include "StateLevel3.hpp"

namespace botwsavs::core {

void StateLevel3::ReadFromGame() {
    ReadMemory(named(mem::GamePtr::TimeOfDayPaused()), &mTimeOfDayPaused);
    ReadMemory(named(mem::GamePtr::TimeOfDayUnpaused()), &mTimeOfDayUnpaused);
    ReadMemory(named(mem::GamePtr::BloodMoonTimer()), &mBloodMoonTimer);
    ReadMemory(named(mem::GamePtr::TemperatureDamageTimer()), &mTemperatureDamageTimer);
    ReadMemory(named(mem::GamePtr::FlameTimer()), &mFlameTimer);
    ReadMemory(named(mem::GamePtr::GaleTimer()), &mGaleTimer);
    ReadMemory(named(mem::GamePtr::FuryTimer()), &mFuryTimer);
    ReadMemory(named(mem::GamePtr::ProtectionTimer()), &mProtectionTimer);
    ReadMemory(named(mem::GamePtr::GraceTimer()), &mGraceTimer);
    ReadMemoryArray(named(mem::GamePtr::AbilityUses()), mAbilityUses, 3);
    ReadMemory(named(mem::GamePtr::MasterSwordCooldown()), &mMasterSwordCooldown);
    ReadMemory(named(mem::GamePtr::SpeedPotionTimer1()), &mSpeedPotionTimer1);
    ReadMemory(named(mem::GamePtr::SpeedPotionTimer2()), &mSpeedPotionTimer2);
    ReadMemory(named(mem::GamePtr::SpeedPotionTimer3()), &mSpeedPotionTimer3);
    ReadMemory(named(mem::GamePtr::AttackPotionTimer()), &mAttackPotionTimer);
    ReadMemory(named(mem::GamePtr::DefensePotionTimer()), &mDefensePotionTimer);
    ReadMemory(named(mem::GamePtr::HeatResistPotionTimer()), &mHeatResistPotionTimer);
    ReadMemory(named(mem::GamePtr::ColdResistPotionTimer()), &mColdResistPotionTimer);
    ReadMemory(named(mem::GamePtr::FlameResistPotionTimer()), &mFlameResistPotionTimer);
    ReadMemory(named(mem::GamePtr::ShockResistPotionTimer()), &mShockResistPotionTimer);
    ReadMemory(named(mem::GamePtr::StealthPotionTimer()), &mStealthPotionTimer);

    uking::ui::PauseMenuDataMgr* pPauseMenuDataMgr;
    if(TakePtrOrError(named(mem::SafePtr(uking::ui::PauseMenuDataMgr::instance())), &pPauseMenuDataMgr)){
        // Save mCount Offset
        s32 count = pPauseMenuDataMgr->getItems().size();
        s32 actualSize = pPauseMenuDataMgr->getItems().sizeSlow();
        mCountOffset = actualSize - count;
    }

}

void StateLevel3::WriteToGame() {

    WriteMemory(named(mem::GamePtr::TimeOfDayPaused()), mTimeOfDayPaused);
    WriteMemory(named(mem::GamePtr::TimeOfDayUnpaused()), mTimeOfDayUnpaused);
    WriteMemory(named(mem::GamePtr::BloodMoonTimer()), mBloodMoonTimer);
    WriteMemory(named(mem::GamePtr::TemperatureDamageTimer()), mTemperatureDamageTimer);
    WriteMemory(named(mem::GamePtr::FlameTimer()), mFlameTimer);
    WriteMemory(named(mem::GamePtr::GaleTimer()), mGaleTimer);
    WriteMemory(named(mem::GamePtr::FuryTimer()), mFuryTimer);
    WriteMemory(named(mem::GamePtr::ProtectionTimer()), mProtectionTimer);
    WriteMemory(named(mem::GamePtr::GraceTimer()), mGraceTimer);
    WriteMemoryArray(named(mem::GamePtr::AbilityUses()), mAbilityUses, 3);
    WriteMemory(named(mem::GamePtr::MasterSwordCooldown()), mMasterSwordCooldown);
    WriteMemory(named(mem::GamePtr::SpeedPotionTimer1()), mSpeedPotionTimer1);
    WriteMemory(named(mem::GamePtr::SpeedPotionTimer2()), mSpeedPotionTimer2);
    WriteMemory(named(mem::GamePtr::SpeedPotionTimer3()), mSpeedPotionTimer3);
    WriteMemory(named(mem::GamePtr::AttackPotionTimer()), mAttackPotionTimer);
    WriteMemory(named(mem::GamePtr::DefensePotionTimer()), mDefensePotionTimer);
    WriteMemory(named(mem::GamePtr::HeatResistPotionTimer()), mHeatResistPotionTimer);
    WriteMemory(named(mem::GamePtr::ColdResistPotionTimer()), mColdResistPotionTimer);
    WriteMemory(named(mem::GamePtr::FlameResistPotionTimer()), mFlameResistPotionTimer);
    WriteMemory(named(mem::GamePtr::ShockResistPotionTimer()), mShockResistPotionTimer);
    WriteMemory(named(mem::GamePtr::StealthPotionTimer()), mStealthPotionTimer);

    uking::ui::PauseMenuDataMgr* pPauseMenuDataMgr;
    if(TakePtrOrError(named(mem::SafePtr(uking::ui::PauseMenuDataMgr::instance())), &pPauseMenuDataMgr)){
        // Set mCount Offset
        s32 actualSize = pPauseMenuDataMgr->getItems().sizeSlow();
        s32 count = actualSize - mCountOffset;
        pPauseMenuDataMgr->getItems().setSize(count);

    }

}

void StateLevel3::ReadFromFile(fs::ConfigFile& file, u32 version) {
    if(version < 3){
        return;
    }
    file.ReadFloat(&mTimeOfDayPaused);
    file.ReadFloat(&mTimeOfDayUnpaused);
    file.ReadFloat(&mBloodMoonTimer);
    file.ReadFloat(&mTemperatureDamageTimer);
    file.ReadFloat(&mFlameTimer);
    file.ReadFloat(&mGaleTimer);
    file.ReadFloat(&mFuryTimer);
    file.ReadFloat(&mProtectionTimer);
    file.ReadFloat(&mGraceTimer);
    file.ReadIntegerArray(mAbilityUses, 3);
    file.ReadFloat(&mMasterSwordCooldown);
    file.ReadFloat(&mSpeedPotionTimer1);
    file.ReadFloat(&mSpeedPotionTimer2);
    file.ReadFloat(&mSpeedPotionTimer3);
    file.ReadFloat(&mAttackPotionTimer);
    file.ReadFloat(&mDefensePotionTimer);
    file.ReadFloat(&mHeatResistPotionTimer);
    file.ReadFloat(&mColdResistPotionTimer);
    file.ReadFloat(&mFlameResistPotionTimer);
    file.ReadFloat(&mShockResistPotionTimer);
    file.ReadFloat(&mStealthPotionTimer);
    
    if (version < 4){
        return;
    }
    // Version 4
    file.ReadInteger(&mCountOffset);

}

void StateLevel3::WriteToFile(fs::ConfigFile& file) const {
    file.WriteFloat(named(mTimeOfDayPaused));
    file.WriteFloat(named(mTimeOfDayUnpaused));
    file.WriteFloat(named(mBloodMoonTimer));
    file.WriteFloat(named(mTemperatureDamageTimer));
    file.WriteFloat(named(mFlameTimer));
    file.WriteFloat(named(mGaleTimer));
    file.WriteFloat(named(mFuryTimer));
    file.WriteFloat(named(mProtectionTimer));
    file.WriteFloat(named(mGraceTimer));
    file.WriteIntegerArray(named(mAbilityUses), 3);
    file.WriteFloat(named(mMasterSwordCooldown));
    file.WriteFloat(named(mSpeedPotionTimer1));
    file.WriteFloat(named(mSpeedPotionTimer2));
    file.WriteFloat(named(mSpeedPotionTimer3));
    file.WriteFloat(named(mAttackPotionTimer));
    file.WriteFloat(named(mDefensePotionTimer));
    file.WriteFloat(named(mHeatResistPotionTimer));
    file.WriteFloat(named(mColdResistPotionTimer));
    file.WriteFloat(named(mFlameResistPotionTimer));
    file.WriteFloat(named(mShockResistPotionTimer));
    file.WriteFloat(named(mStealthPotionTimer));
    // Version 4
    file.WriteInteger(named(mCountOffset));
}



}  // namespace botwsavs::core

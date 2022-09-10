#include "fs/ConfigFile.hpp"
#include "fs/Logger.hpp"
#include "KeyMgr.hpp"

namespace botwsavs::core {

bool KeyMgr::DidHoldFor(u32 keys, u32 seconds) {
    if (mHoldingKeys != keys) {
        mHoldCounter = 0;
        mHoldingKeys = keys;
    } else {
        mHoldCounter++;
    }
    // 1 tick = 3 frames
    // 1s = 30 frames = 10 ticks
    u32 ticks = seconds * 10;
    if (mHoldCounter >= ticks) {
        mHoldCounter = 0;
        return true;
    }
    return false;
}

void KeyMgr::StartConfigure(u32* configureKey){
    info("Configuring key binding");
    ClearHold();
    mpConfiguringKey = configureKey;
    mConfigureCounter = 0;
}

KeyMgr::ConfigureResult KeyMgr::FinishConfigure(u32 newKey){
    if (newKey == 0){
        mConfigureCounter = 0;
        return ConfigureResult::Pending;
    }
    if (mConfigureCounter < 30){
        mConfigureCounter++;
        return ConfigureResult::Pending;
    }
    
    
    ClearHold();
    if (newKey == 0){
        mpConfiguringKey = nullptr;
        return ConfigureResult::FailEmpty;
    }
    *mpConfiguringKey = newKey;
    mpConfiguringKey = nullptr;
    infof("New combo: %x", newKey);
    return ConfigureResult::Success;

}

void KeyMgr::Save(fs::ConfigFile& file) const {
    file.WriteInteger(named(mKeySave));
    file.WriteInteger(named(mKeySaveFile));
    file.WriteInteger(named(mKeyRestore));
    file.WriteInteger(named(mKeyRestoreFile));
}

void KeyMgr::Load(fs::ConfigFile& file) {
    file.ReadInteger(&mKeySave);
    file.ReadInteger(&mKeySaveFile);
    file.ReadInteger(&mKeyRestore);
    file.ReadInteger(&mKeyRestoreFile);
}

}

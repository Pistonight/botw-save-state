#pragma once

#include "Key.hpp"

namespace botwsavs {

namespace fs {
class ConfigFile;
}

namespace core {
// class for managing key combos and key presses
class KeyMgr {

public:

    enum class ConfigureResult : u32 {
        Success = 0,
        Pending = 1,
        FailEmpty = 2,
        FailRelease = 3,
    };

    bool DidHoldFor(u32 keys, u32 seconds);
    void ClearHold() {
        mHoldingKeys = 0;
        mHoldCounter = 0;
    }
    void Save(fs::ConfigFile& file) const;
    void Load(fs::ConfigFile& file);

    bool IsConfiguring() const { return mpConfiguringKey != nullptr; }
    void StartConfigure(u32* configureKey);
    ConfigureResult FinishConfigure(u32 newKey);

public:
    // Configurable
    u32 mKeySave = Key::ZL | Key::L | Key::Plus | Key::DpadLeft;
    u32 mKeySaveFile = Key::ZL | Key::L | Key::Plus | Key::DpadLeft | Key::RStick;
    u32 mKeyRestore = Key::ZL | Key::L | Key::Plus | Key::DpadRight;
    u32 mKeyRestoreFile = Key::ZL | Key::L | Key::Plus | Key::DpadRight | Key::RStick;
    
    // Not configurable
    u32 mKeySwitchMode = Key::DpadDown | Key::ZL | Key::ZR | Key::L | Key::R;
    u32 mKeyIncreaseLevel = Key::R;
    u32 mKeyDecreaseLevel = Key::L;

private:
    u32 mHoldCounter = 0;
    u32 mHoldingKeys = 0;
    u32* mpConfiguringKey = nullptr;
    u32 mConfigureCounter = 0;
};

}
}


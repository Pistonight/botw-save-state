#pragma once

#include "fs/ConfigFile.hpp"

#include "Controller.hpp"
#include "KeyMgr.hpp"
#include "State.hpp"

#define WORKER_TXT_PATH "sd:/botwsavs/worker.txt"

namespace botwsavs {

namespace core {

class Worker : fs::Config {
public:
    enum class Mode { Active, Setting };
    // enum class Hold {
    //     None,
    //     ModeSwitch,
    //     IncreaseLevel,
    //     DecreaseLevel,
    // };

public:
    bool Init();
    // Return if work is successful
    bool Work() {
        if (!CanWork()) {
            return false;
        }
        if (TrySwitchMode()) {
            return true;
        }
        if (mMode == Mode::Active) {
            return WorkActiveMode();
        } else {
            return WorkSettingMode();
        }
    }

    void Save(fs::ConfigFile& configFile) const override;
    void Load(fs::ConfigFile& configFile) override;

private:
    // Core
    bool CanWork() { return mController.IsInitialized(); }
    bool WorkActiveMode();
    bool WorkSettingMode();
    bool TrySwitchMode();

    // Save state to memory
    void ExecuteSave();
    // Save state to file
    void ExecuteSaveToFile();
    // Restore state from memory
    void ExecuteRestore();
    // Restore state from file
    void ExecuteRestoreFromFile();
    // Display the most important state error
    void DisplayStateError(State& state);
    // Save worker config
    bool SaveConfig() const;
    // Load worker config
    bool LoadConfig();

public:
    u32 mLevel = 1;

private:
    Controller mController;
    KeyMgr mKeyMgr;
    State mState;
    Mode mMode = Mode::Active;
};

}  // namespace core

}  // namespace botwsavs

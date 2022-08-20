#pragma once
#include "Controller.hpp"
#include "State.hpp"

namespace botwsavs {

namespace core {

class Worker {
public:
    enum class Mode { Active, Setting };
    enum class Hold {
        None,
        ModeSwitch,
        IncreaseLevel,
        DecreaseLevel,
    };

public:
    // Return if work is successful
    bool Work();

private:
    bool DidHoldFor(Hold hold, u32 seconds);
    // Save state to memory
    void ExecuteSave();
    // Save state to file
    void ExecuteSaveToFile();
    // Restore state from memory
    void ExecuteRestore();
    // Restore state from file
    void ExecuteRestoreFromFile();

private:
    Controller mController;
    State mState;
    Mode mMode = Mode::Active;
    u32 mLevel = 1;
    u32 mHoldCounter = 0;
    Hold mHold = Hold::None;
};

}  // namespace core

}  // namespace botwsavs

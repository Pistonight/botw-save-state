#pragma once

#include "types.h"

namespace sead {
struct Controller;
}

namespace botwsavs {

namespace core {
class Controller {
public:
    enum Key : u32 {
        B = 1 << 1,
        ZL = 1 << 2,
        ZR = 1 << 5,
        RStick = 1 << 6,
        Plus = 1 << 10,

        L = 1 << 13,
        R = 1 << 14,
        DpadDown = 1 << 17,
        DpadLeft = 1 << 18,
        DpadRight = 1 << 19
    };

public:
    Controller() = default;
    ~Controller();

    bool isInitialized() {
        if (!mpController) {
            TryGetController();
        }
        return mpController != nullptr;
    }

    bool ShouldSaveState();
    bool ShouldRestoreState();
    bool ShouldSaveStateToFile();
    bool ShouldRestoreStateFromFile();
    bool ShouldSwitchMode();
    bool ShouldIncreaseLevel();
    bool ShouldDecreaseLevel();

private:
    // Return true if controller is cached succesfully
    bool TryGetController();

private:
    sead::Controller* mpController = nullptr;
};

}  // namespace core
}  // namespace botwsavs

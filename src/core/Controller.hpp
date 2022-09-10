#pragma once

#include "types.h"

namespace sead {
struct Controller;
}

namespace botwsavs {

namespace core {
class Controller {
public:
    Controller() = default;
    ~Controller() { mpController = nullptr; }

    bool IsInitialized() {
        if (!mpController) {
            TryGetController();
        }
        return mpController != nullptr;
    }

    bool IsOnlyHolding(u32 mask);

    u32 GetHoldKeys();

private:
    // Return true if controller is cached succesfully
    bool TryGetController();

private:
    sead::Controller* mpController = nullptr;
};

}  // namespace core
}  // namespace botwsavs

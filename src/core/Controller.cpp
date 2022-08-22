#include "Controller.hpp"
#include <controller/seadController.h>
#include <controller/seadControllerMgr.h>
#include "fs/Logger.hpp"
#include "mem/KingPtr.hpp"

namespace botwsavs::core {

Controller::~Controller() {
    mpController = nullptr;
}

bool Controller::TryGetController() {
    if (mpController) {
        return true;
    }
    info("Try accessing controller instance");

    auto* pControllerMgr = mem::KingPtr::sControllerMgrInstance;
    if (!pControllerMgr) {
        error("Failed to access ControllerMgr: nullptr");
        return false;
    }
    sead::Controller* pController = pControllerMgr->getController(0);
    if (!pController) {
        error("Failed to find controller instance");
        return false;
    }

    fs::FileBuffer buf;
    buf.SafeAppendF("Found controller instance: %p", pController);
    info(buf);

    mpController = pController;
    return true;
}

bool Controller::ShouldSaveState() {
    return mpController->isHoldAll(Key::Plus | Key::DpadLeft) && !mpController->isHold(Key::RStick);
}

bool Controller::ShouldRestoreState() {
    return mpController->isHoldAll(Key::Plus | Key::DpadRight) &&
           !mpController->isHold(Key::RStick);
}

bool Controller::ShouldSaveStateToFile() {
    return mpController->isHoldAll(Key::Plus | Key::DpadLeft | Key::RStick);
}

bool Controller::ShouldRestoreStateFromFile() {
    return mpController->isHoldAll(Key::Plus | Key::DpadRight | Key::RStick);
}

bool Controller::ShouldSwitchMode() {
    return mpController->isHoldAll(Key::DpadDown | Key::ZL | Key::ZR | Key::L | Key::R);
}

bool Controller::ShouldIncreaseLevel() {
    return mpController->isHold(Key::R);
}
bool Controller::ShouldDecreaseLevel() {
    return mpController->isHold(Key::L);
}

}  // namespace botwsavs::core

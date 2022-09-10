#include "Controller.hpp"
#include <controller/seadController.h>
#include <controller/seadControllerMgr.h>
#include "fs/Logger.hpp"
#include "mem/KingPtr.hpp"

namespace botwsavs::core {

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

    infof("Found controller instance: %p", pController);

    mpController = pController;
    return true;
}

bool Controller::IsOnlyHolding(u32 mask){
    return mpController->isHoldAll(mask) && !mpController->isHold(~mask);
}

u32 Controller::GetHoldKeys(){
    return mpController->getHoldMask();
}

}  // namespace botwsavs::core

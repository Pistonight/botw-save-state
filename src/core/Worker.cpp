#include <prim/seadSafeString.h>

#include "Worker.hpp"
#include "fs/Logger.hpp"
#include "fs/SaveFile.hpp"
#include "ui/OverlayString.hpp"

namespace botwsavs::core {

bool Worker::Work() {
    bool controllerReady = mController.isInitialized();
    if (!controllerReady) {
        return false;
    }
    if (mController.ShouldSwitchMode()) {
        if (DidHoldFor(Hold::ModeSwitch, 4)) {
            info("Switching mode");
            if (mMode == Mode::Active) {
                mMode = Mode::Setting;
                ui::ShowOverridenMessage("Save State: Setting Mode");
            } else {
                mMode = Mode::Active;
                ui::ShowOverridenMessage("Save State: Active Mode");
            }
            mHold = Hold::None;
        }
    } else if (mMode == Mode::Active) {
        if (mController.ShouldSaveState()) {
            ExecuteSave();
        } else if (mController.ShouldRestoreState()) {
            ExecuteRestore();
        } else if (mController.ShouldSaveStateToFile()) {
            ExecuteSaveToFile();
        } else if (mController.ShouldRestoreStateFromFile()) {
            ExecuteRestoreFromFile();
        }
        mHold = Hold::None;
    } else {
        if (mController.ShouldIncreaseLevel()) {
            if (DidHoldFor(Hold::IncreaseLevel, 1)) {
                info("Increase level");
                if (mLevel < 3) {
                    mLevel++;
                }
                ui::ShowSetLevel(mLevel);
                mHold = Hold::None;
            }
        } else if (mController.ShouldDecreaseLevel()) {
            if (DidHoldFor(Hold::DecreaseLevel, 1)) {
                info("Decrease level");
                if (mLevel > 0) {
                    mLevel--;
                }
                ui::ShowSetLevel(mLevel);
                mHold = Hold::None;
            }
        } else {
            mHold = Hold::None;
        }
    }

    return true;
}

bool Worker::DidHoldFor(Hold hold, u32 seconds) {
    if (mHold != hold) {
        mHoldCounter = 0;
        mHold = hold;
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

void Worker::ExecuteSave() {
    info("Saving state to memory");
    if (mLevel == 0) {
        error("Save failed because setting level is 0");
        return;
    }
    if (!mState.ReadFromGame(mLevel)) {
        ui::ShowError();
        error("State read failed");
        return;
    }
    ui::ShowOverridenMessage("Saved state to memory");
    info("State saved to memory");
}

void Worker::ExecuteSaveToFile() {
    info("Saving state to file");
    if (mLevel == 0) {
        error("Save failed because setting level is 0");
        return;
    }
    State tempState;
    if (!tempState.ReadFromGame(mLevel)) {
        ui::ShowError();
        error("State read failed");
        return;
    }
    fs::SaveFile latestTxt("sd:/botwsavs/latest.txt");
    bool result = latestTxt.Save(tempState);

    if (!result) {
        ui::ShowError();
        error("File operation failed");
        return;
    }

    ui::ShowOverridenMessage("Saved state to file");
    info("State saved to file");
}
void Worker::ExecuteRestore() {
    info("Restoring state from memory");

    if (mLevel == 0) {
        error("Restore failed because setting level is 0");
        return;
    }

    if (mState.mLevel < mLevel) {
        error("Restore failed because state level is less than setting level");
        ui::ShowLevelError();
        return;
    }

    if (!mState.WriteToGame()) {
        ui::ShowError();
        error("State write failed");
        return;
    }
#ifndef GOLD_RUSH
    // Don't show restore message for gold rush
    ui::ShowOverridenMessage("Restored state from memory");
#endif
    info("State restored from memory");
}

void Worker::ExecuteRestoreFromFile() {
    info("Restoring state from file");
    if (mLevel == 0) {
        error("Restore failed because setting level is 0");
        return;
    }

    State tempState;
    fs::SaveFile restoreTxt("sd:/botwsavs/restore.txt");
    if (!restoreTxt.Exists()) {
        error("Restore failed because restore.txt does not exist");
        ui::ShowCantDoThatRightNow();
        return;
    }

    bool result = restoreTxt.Load(tempState);
    if (!result) {
        ui::ShowError();
        error("File operation failed");
        return;
    }
    if (tempState.mLevel < mLevel) {
        error("Restore failed because state level is less than setting level");
        ui::ShowLevelError();
        return;
    }

    if (!tempState.WriteToGame()) {
        ui::ShowError();
        error("State write failed");
        return;
    }

#ifndef GOLD_RUSH
    // Don't show restore message for gold rush
    ui::ShowOverridenMessage("Restored state from file");
#endif
    info("State restored from file");
}

}  // namespace botwsavs::core

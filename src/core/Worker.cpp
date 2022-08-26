#include <prim/seadSafeString.h>

#include "Worker.hpp"
#include "fs/Logger.hpp"
#include "fs/StateSaveFile.hpp"
#include "fs/WorkerSaveFile.hpp"
#include "ui/OverlayString.hpp"

namespace botwsavs::core {

bool Worker::Init() {
    info("Init worker config from file");

    fs::WorkerSaveFile workerTxt(*this);
    bool result = workerTxt.Load();

    if (!result) {
        warn("File operation failed. Cannot init worker");
        return false;
    }
    return true;
}

bool Worker::Work() {
    bool controllerReady = mController.isInitialized();
    if (!controllerReady) {
        return false;
    }
    if (mController.ShouldSwitchMode()) {
        if (DidHoldFor(Hold::ModeSwitch, 3)) {
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
                    SaveWorker();
                }
                ui::ShowSetLevel(mLevel);
                mHold = Hold::None;
            }
        } else if (mController.ShouldDecreaseLevel()) {
            if (DidHoldFor(Hold::DecreaseLevel, 1)) {
                info("Decrease level");
                if (mLevel > 0) {
                    mLevel--;
                    SaveWorker();
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
        DisplayStateError(mState);
        error("State read failed");
        return;
    }
    ui::ShowFormattedMessage("Saved state to memory (level %d)", mLevel);
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
        DisplayStateError(tempState);
        warnf("State read gives error 0x%x, but continuing to write file anyway",
              tempState.GetError());
    }
    fs::StateSaveFile latestTxt("sd:/botwsavs/latest.txt", tempState);
    bool result = latestTxt.Save();

    if (!result) {
        ui::ShowError();
        error("File operation failed");
        return;
    }

    ui::ShowFormattedMessage("Saved state to file (level %d)", mLevel);
    info("State saved to file");
}
void Worker::ExecuteRestore() {
    info("Restoring state from memory");

    if (mLevel == 0) {
        error("Restore failed because setting level is 0");
        return;
    }

    if (mState.mLevel == 0) {
        error("Restore failed because state level is 0 (nothing stored)");
        ui::ShowCantDoThatRightNow();
        return;
    }

    if (mState.mLevel < mLevel) {
        error("Restore failed because state level is less than setting level");
        ui::ShowLevelError(mState.mLevel);
        return;
    }

    if (!mState.WriteToGame(mLevel)) {
        DisplayStateError(mState);
        error("State write failed");
        return;
    }
#ifndef GOLD_RUSH
    // Don't show restore message for gold rush
    ui::ShowFormattedMessage("Restored state from memory (level %d)", mLevel);
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
    fs::StateSaveFile restoreTxt("sd:/botwsavs/restore.txt", tempState);
    if (!restoreTxt.Exists()) {
        error("Restore failed because restore.txt does not exist");
        ui::ShowCantDoThatRightNow();
        return;
    }

    bool result = restoreTxt.Load();
    if (!result) {
        ui::ShowError();
        error("File operation failed");
        return;
    }
    if (tempState.mLevel < mLevel) {
        error("Restore failed because state level is less than setting level");
        ui::ShowLevelError(tempState.mLevel);
        return;
    }

    if (!tempState.WriteToGame(mLevel)) {
        DisplayStateError(tempState);
        error("State write failed");
        return;
    }

#ifndef GOLD_RUSH
    // Don't show restore message for gold rush
    ui::ShowFormattedMessage("Restored state from file (level %d)", mLevel);
#endif
    info("State restored from file");
}

void Worker::DisplayStateError(State& state) {
    if (state.HasError(State::Error::DifferentName)) {
        ui::ShowOverridenMessage("Warning: Some equipped items are different!");
        return;
    }

    if (state.HasError(State::Error::NotEquipped)) {
        ui::ShowOverridenMessage("Warning: Something not equipped?");
        return;
    }

    if (state.HasError(State::Error::Pointer)) {
        ui::ShowOverridenMessage("Pointer error!");
    }
}

void Worker::SaveWorker() {
    info("Saving worker config to file");

    fs::WorkerSaveFile workerTxt(*this);
    bool result = workerTxt.Save();

    if (!result) {
        warn("File operation failed. Cannot save worker");
    }
}

}  // namespace botwsavs::core

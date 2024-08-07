
#include "Worker.hpp"
#include "fs/Logger.hpp"
#include "ui/OverlayString.hpp"

namespace botwsavs::core {

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
              tempState.GetErrorMask());
    }

    if (!tempState.WriteToFile()) {
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

    if (mState.GetLevel() == 0) {
        error("Restore failed because state level is 0 (nothing stored)");
        ui::ShowCantDoThatRightNow();
        return;
    }

    if (mState.GetLevel() < mLevel) {
        error("Restore failed because state level is less than setting level");
        ui::ShowLevelError(mState.GetLevel());
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
    if (!tempState.CanReadFromFile()) {
        error("Restore failed because restore.txt does not exist");
        ui::ShowCantDoThatRightNow();
        return;
    }

    if (!tempState.ReadFromFile()) {
        ui::ShowError();
        error("File operation failed");
        return;
    }
    if (tempState.GetLevel() < mLevel) {
        error("Restore failed because state level is less than setting level");
        ui::ShowLevelError(tempState.GetLevel());
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
    if (state.HasError(StateError::DifferentName)) {
        ui::ShowOverridenMessage("Warning: Some equipped items are different!");
        return;
    }

    if (state.HasError(StateError::NotEquipped)) {
        ui::ShowOverridenMessage("Warning: Something not equipped?");
        return;
    }

    if (state.HasError(StateError::Pointer)) {
        ui::ShowOverridenMessage("Pointer error!");
    }
}

}  // namespace botwsavs::core

#include "Worker.hpp"
#include "fs/Logger.hpp"
#include "ui/OverlayString.hpp"

namespace botwsavs::core {

bool Worker::Init() {
    info("Init worker");

    return LoadConfig();
}

bool Worker::TrySwitchMode() {
    if (mController.IsOnlyHolding(mKeyMgr.mKeySwitchMode)) {
        if (mKeyMgr.DidHoldFor(mKeyMgr.mKeySwitchMode, 3)) {
            info("Switching mode");
            if (mMode == Mode::Active) {
                mMode = Mode::Setting;
                ui::ShowOverridenMessage("Save State: Setting Mode");
            } else {
                mMode = Mode::Active;
                ui::ShowOverridenMessage("Save State: Active Mode");
            }
            mKeyMgr.ClearHold();
        }
        return true;
    }

    return false;
}
bool Worker::WorkActiveMode() {
    if (mController.IsOnlyHolding(mKeyMgr.mKeySave)) {
        ExecuteSave();
    } else if (mController.IsOnlyHolding(mKeyMgr.mKeySaveFile)) {
        ExecuteSaveToFile();
    } else if (mController.IsOnlyHolding(mKeyMgr.mKeyRestore)) {
        ExecuteRestore();
    } else if (mController.IsOnlyHolding(mKeyMgr.mKeyRestoreFile)) {
        ExecuteRestoreFromFile();
    }

    return true;
}

bool Worker::WorkSettingMode() {
    if (mKeyMgr.IsConfiguring()) {
        u32 newKey = mController.GetHoldKeys();
        KeyMgr::ConfigureResult result = mKeyMgr.FinishConfigure(newKey);
        switch (result) {
        case KeyMgr::ConfigureResult::Success: {
            util::StringBuffer<120> buffer;
            key::GetKeyString(newKey, buffer);
            ui::ShowFormattedMessage("Configured: %s", buffer.Content());
            SaveConfig();
            break;
        }
        case KeyMgr::ConfigureResult::FailEmpty:
            ui::ShowOverridenMessage("Key binding cannot be updated to none!");
            break;
        case KeyMgr::ConfigureResult::FailRelease:
            ui::ShowOverridenMessage("Holding keys changed, key binding change failed!");
            break;
        case KeyMgr::ConfigureResult::Pending:
        default:
            break;
        }
        return true;
    }

    if (mController.IsOnlyHolding(mKeyMgr.mKeyIncreaseLevel)) {
        if (mKeyMgr.DidHoldFor(mKeyMgr.mKeyIncreaseLevel, 1)) {
            info("Increase level");
            if (mLevel < 3) {
                mLevel++;
                SaveConfig();
            }
            ui::ShowSetLevel(mLevel);
            mKeyMgr.ClearHold();
        }
    } else if (mController.IsOnlyHolding(mKeyMgr.mKeyDecreaseLevel)) {
        if (mKeyMgr.DidHoldFor(mKeyMgr.mKeyDecreaseLevel, 1)) {
            info("Decrease level");
            if (mLevel > 0) {
                mLevel--;
                SaveConfig();
            }
            ui::ShowSetLevel(mLevel);
            mKeyMgr.ClearHold();
        }
    } else if (mController.IsOnlyHolding(mKeyMgr.mKeySave)) {
        if (mKeyMgr.DidHoldFor(mKeyMgr.mKeySave, 3)) {
            ui::ShowSetKeyBinding("memory save");
            mKeyMgr.StartConfigure(&mKeyMgr.mKeySave);
        }
    } else if (mController.IsOnlyHolding(mKeyMgr.mKeySaveFile)) {
        if (mKeyMgr.DidHoldFor(mKeyMgr.mKeySaveFile, 3)) {
            ui::ShowSetKeyBinding("file save");
            mKeyMgr.StartConfigure(&mKeyMgr.mKeySaveFile);
        }
    } else if (mController.IsOnlyHolding(mKeyMgr.mKeyRestore)) {
        if (mKeyMgr.DidHoldFor(mKeyMgr.mKeyRestore, 3)) {
            ui::ShowSetKeyBinding("memory restore");
            mKeyMgr.StartConfigure(&mKeyMgr.mKeyRestore);
        }
    } else if (mController.IsOnlyHolding(mKeyMgr.mKeyRestoreFile)) {
        if (mKeyMgr.DidHoldFor(mKeyMgr.mKeyRestoreFile, 3)) {
            ui::ShowSetKeyBinding("file restore");
            mKeyMgr.StartConfigure(&mKeyMgr.mKeyRestoreFile);
        }
    } else {
        mKeyMgr.ClearHold();
    }

    return true;
}
}  // namespace botwsavs::core

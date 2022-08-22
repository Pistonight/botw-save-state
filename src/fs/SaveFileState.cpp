#include <stdlib.h>
#include "core/State.hpp"
#include "util/StringBuffer.hpp"

#include "Logger.hpp"
#include "SaveFile.hpp"
#include "SaveFileMacros.hpp"

namespace botwsavs::fs {

bool SaveFile::Load(core::State& state) {
    bool result = mFile.Open();
    if (!result) {
        return false;
    }

    FileBuffer buffer;
    u32 version;
    result = ReadInteger(buffer, &version);
    if (!result) {
        return false;
    }
    switch (version) {
    case 1:
        result = ReadVersion1(buffer, state);
        break;
    case 2:
        result = ReadVersion2(buffer, state);
        break;
    }

    mFile.Close();
    return result;
}

bool SaveFile::Save(const core::State& state) {
    bool result = mFile.Open();
    if (!result) {
        return false;
    }

    result = mFile.Clear();
    if (!result) {
        mFile.Close();
        return false;
    }

    FileBuffer buffer;
    result = WriteInteger(buffer, "version", SAVE_FILE_VERSION);
    SaveFileWrite(Integer, mLevel);
    // Level 1
    SaveFileWrite(Integer, mHealth);
    SaveFileWrite(Float, mStamina);
    SaveFileWriteArray(Float, mHavokPosition, 3);
    SaveFileWriteArray(Float, mMainPositionMatrix, 12);
    SaveFileWriteArray(Float, mCameraPanMatrix, 12);
    SaveFileWrite(Float, mCameraZoom);
    SaveFileWrite(Float, mCameraTilt);
    // Level 2
    SaveFileWriteNamedValue(Integer, mMenuEquippedArrow, 64);
    SaveFileWriteNamedValue(Integer, mMenuEquippedWeapon, 64);
    SaveFileWriteNamedValue(Integer, mMenuEquippedBow, 64);
    SaveFileWriteNamedValue(Integer, mMenuEquippedShield, 64);
    SaveFileWriteNamedValue(Integer, mOverworldEquippedWeapon, 64);
    SaveFileWriteNamedValue(Integer, mOverworldEquippedBow, 64);
    SaveFileWriteNamedValue(Integer, mOverworldEquippedShield, 64);
    // Level 3
    // TODO
    mFile.Close();
    return result;
}

bool SaveFile::ReadVersion1(FileBuffer& buffer, core::State& state) {
    bool result = true;
    SaveFileRead(Integer, mLevel);
    SaveFileRead(Integer, mHealth);
    SaveFileRead(Float, mStamina);
    SaveFileReadArray(Float, mHavokPosition, 3);
    SaveFileReadArray(Float, mMainPositionMatrix, 12);
    SaveFileReadArray(Float, mCameraPanMatrix, 12);
    SaveFileRead(Float, mCameraZoom);
    SaveFileRead(Float, mCameraTilt);

    return result;
}

bool SaveFile::ReadVersion2(FileBuffer& buffer, core::State& state) {
    bool result = ReadVersion1(buffer, state);
    SaveFileReadNamedValue(Integer, mMenuEquippedArrow, 64);
    SaveFileReadNamedValue(Integer, mMenuEquippedWeapon, 64);
    SaveFileReadNamedValue(Integer, mMenuEquippedBow, 64);
    SaveFileReadNamedValue(Integer, mMenuEquippedShield, 64);
    SaveFileReadNamedValue(Integer, mOverworldEquippedWeapon, 64);
    SaveFileReadNamedValue(Integer, mOverworldEquippedBow, 64);
    SaveFileReadNamedValue(Integer, mOverworldEquippedShield, 64);

    return result;
}

}  // namespace botwsavs::fs

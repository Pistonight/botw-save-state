#include <stdlib.h>
#include "core/State.hpp"
#include "fs/Logger.hpp"
#include "util/StringBuffer.hpp"

#include "SaveFile.hpp"

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
    }

    mFile.Close();
    return result;
}

#define mWrite(TYPE, MEMBER) result = result && Write##TYPE(buffer, #MEMBER, state.MEMBER);
#define mWriteArray(TYPE, MEMBER, SIZE)                                                            \
    result = result && Write##TYPE##Array(buffer, #MEMBER, state.MEMBER, SIZE);

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
    result = WriteInteger(buffer, "version", 1);
    mWrite(Integer, mLevel);
    mWrite(Integer, mHealth);
    mWrite(Float, mStamina);
    mWriteArray(Float, mHavokPosition, 3);
    mWriteArray(Float, mMainPositionMatrix, 12);
    mWriteArray(Float, mCameraPanMatrix, 12);
    mWrite(Float, mCameraZoom);
    mWrite(Float, mCameraTilt);

    mFile.Close();
    return result;
}

#define mRead(TYPE, MEMBER) result = result && Read##TYPE(buffer, &state.MEMBER);
#define mReadArray(TYPE, MEMBER, SIZE)                                                             \
    result = result && Read##TYPE##Array(buffer, state.MEMBER, SIZE);

bool SaveFile::ReadVersion1(FileBuffer& buffer, core::State& state) {
    bool result = true;
    mRead(Integer, mLevel);
    mRead(Integer, mHealth);
    mRead(Float, mStamina);
    mReadArray(Float, mHavokPosition, 3);
    mReadArray(Float, mMainPositionMatrix, 12);
    mReadArray(Float, mCameraPanMatrix, 12);
    mRead(Float, mCameraZoom);
    mRead(Float, mCameraTilt);
    mRead(Integer, mLevel);

    return result;
}

bool SaveFile::ReadLine(FileBuffer& buffer, u32* outLineLength) {
    u32 lineLength;
    if (!buffer.IndexOf('\n', 0, &lineLength)) {
        // no new line character found, try reading more
        s64 readSize = mFile.Read(buffer);
        if (readSize < 0) {
            return false;
        }

        if (!buffer.IndexOf('\n', 0, &lineLength)) {
            // still no new line, probably error
            return false;
        }
    }

    buffer.SafeSet(lineLength, '\0');
    // find a comment character
    u32 commentIndex;
    if (buffer.IndexOf('#', 0, &commentIndex)) {
        // remove comment
        buffer.SafeSet(commentIndex, '\0');
    }

    *outLineLength = lineLength;
    return true;
}

bool SaveFile::WriteInteger(FileBuffer& buffer, const char* fieldName, u64 value) {
    buffer.Clear();
    buffer.SafeAppendF("0x%016x", value);
    buffer.SafeAppendF("# %s\n", fieldName);
    return mFile.Write(buffer);
}

bool SaveFile::ReadInteger(FileBuffer& buffer, u64* outValue) {
    u32 lineLength;
    if (!ReadLine(buffer, &lineLength)) {
        return false;
    }

    // Convert to hex
    u64 value = strtol(buffer.Content(), nullptr, 0);
    buffer.SafeDeleteFront(lineLength + 1);
    *outValue = value;
    return true;
}

}  // namespace botwsavs::fs

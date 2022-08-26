#include "ConfigFile.hpp"
#include <stdlib.h>
#include "Logger.hpp"
//#include "util/StringBuffer.hpp"

namespace botwsavs::fs {

bool ConfigFile::Save() {
    if (!mFile.Open()) {
        return false;
    }

    if (!mFile.Clear()) {
        mFile.Close();
        return false;
    }
    mSuccess = true;
    mMode = Mode::Save;
    SaveInternal();
    mFile.Close();
    return mSuccess;
}

bool ConfigFile::Load() {
    if (!mFile.Open()) {
        return false;
    }
    mSuccess = true;
    mMode = Mode::Load;
    mBuffer.Clear();
    LoadInternal();
    mFile.Close();
    return mSuccess;
}

bool ConfigFile::ReadLine(u32* outLineLength) {
    u32 lineLength;
    if (!mBuffer.IndexOf('\n', 0, &lineLength)) {
        // no new line character found, try reading more
        s64 readSize = mFile.Read(mBuffer);
        if (readSize < 0) {
            error("Reached end of file");
            return false;
        }

        if (!mBuffer.IndexOf('\n', 0, &lineLength)) {
            // still no new line, probably error
            error("Line is too long and cannot fit in buffer");
            return false;
        }
    }

    mBuffer.SafeSet(lineLength, '\0');
    // find a comment character
    u32 commentIndex;
    if (mBuffer.IndexOf('#', 0, &commentIndex)) {
        // remove comment
        mBuffer.SafeSet(commentIndex, '\0');
    }

    *outLineLength = lineLength;
    return true;
}

bool ConfigFile::WriteIntegerInternal(const char* fieldName, u64 value) {
    if (mMode == Mode::Load) {
        error("Cannot write in load mode");
        return false;
    }
    mBuffer.Clear();
    mBuffer.SafeAppendF("0x%016x", value);
    mBuffer.SafeAppendF("# %s\n", fieldName);
    return mFile.Write(mBuffer);
}

bool ConfigFile::ReadIntegerInternal(u64* outValue) {
    if (mMode == Mode::Save) {
        error("Cannot read in save mode");
        return false;
    }
    u32 lineLength;
    if (!ReadLine(&lineLength)) {
        return false;
    }

    // Convert to hex
    u64 value = strtol(mBuffer.Content(), nullptr, 0);
    mBuffer.SafeDeleteFront(lineLength + 1);
    *outValue = value;
    return true;
}

bool ConfigFile::WriteStringInternal(const char* fieldName, const char* string) {
    if (mMode == Mode::Load) {
        error("Cannot write in load mode");
        return false;
    }
    mBuffer.Clear();
    mBuffer.SafeAppendF("%s", string);
    mBuffer.SafeAppendF("# %s\n", fieldName);
    return mFile.Write(mBuffer);
}

bool ConfigFile::ReadStringInternal(char* outString, const u32 bufferLength) {
    if (mMode == Mode::Save) {
        error("Cannot read in save mode");
        return false;
    }
    u32 lineLength;
    if (!ReadLine(&lineLength)) {
        return false;
    }
    strncpy(outString, mBuffer.Content(), bufferLength);
    mBuffer.SafeDeleteFront(lineLength + 1);
    outString[bufferLength - 1] = '\0';

    return true;
}

}  // namespace botwsavs::fs

#include "SaveFile.hpp"
#include <stdlib.h>
#include "util/StringBuffer.hpp"

namespace botwsavs::fs {

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

bool SaveFile::WriteString(FileBuffer& buffer, const char* fieldName, const char* string,
                           const u32 bufferLength) {
    buffer.Clear();
    buffer.SafeAppendF("%s", string);
    buffer.SafeAppendF("# %s\n", fieldName);
    return mFile.Write(buffer);
}

bool SaveFile::ReadString(FileBuffer& buffer, char* outString, const u32 bufferLength) {
    u32 lineLength;
    if (!ReadLine(buffer, &lineLength)) {
        return false;
    }
    strncpy(outString, buffer.Content(), bufferLength);
    buffer.SafeDeleteFront(lineLength + 1);
    outString[bufferLength - 1] = '\0';

    return true;
}

}  // namespace botwsavs::fs

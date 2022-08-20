#pragma once

#include "File.hpp"
namespace botwsavs {
namespace core {
class State;
}
namespace fs {

class SaveFile {
public:
    SaveFile(const char* path) : mFile(path) {}
    ~SaveFile() = default;

    bool Save(const core::State& state);
    bool Load(core::State& state);
    bool Exists() { return mFile.Exists(); }

private:
    File mFile;

    bool ReadVersion1(FileBuffer& buffer, core::State& state);

    bool ReadLine(FileBuffer& buffer, u32* outLineLength);

    bool WriteInteger(FileBuffer& buffer, const char* fieldName, const u64 value);

    bool ReadInteger(FileBuffer& buffer, u64* outValue);

    bool ReadInteger(FileBuffer& buffer, u32* outValue) {
        u64 iValue;
        if (!ReadInteger(buffer, &iValue)) {
            return false;
        }

        u32 truncated = (u32)(iValue);
        *outValue = truncated;
        return true;
    }

    bool WriteFloat(FileBuffer& buffer, const char* fieldName, const f32 value) {
        return WriteInteger(buffer, fieldName, reinterpret_cast<const u32&>(value));
    }

    bool ReadFloat(FileBuffer& buffer, f32* value) {
        u32 iValue;
        if (!ReadInteger(buffer, &iValue)) {
            return false;
        }
        *value = reinterpret_cast<f32&>(iValue);
        return true;
    }

    template <typename T>
    bool WriteIntegerArray(FileBuffer& buffer, const char* fieldName, const T* array, u32 size) {
        if (!WriteInteger(buffer, fieldName, array[0])) {
            return false;
        }
        for (u32 i = 1; i < size; i++) {
            if (!WriteInteger(buffer, " ---", array[i])) {
                return false;
            }
        }
        return true;
    }
    template <typename T>
    bool ReadIntegerArray(FileBuffer& buffer, T* outArray, u32 size) {
        if (!ReadInteger(buffer, outArray)) {
            return false;
        }
        for (u32 i = 1; i < size; i++) {
            if (!ReadInteger(buffer, outArray + i)) {
                return false;
            }
        }
        return true;
    }

    bool WriteFloatArray(FileBuffer& buffer, const char* fieldName, const f32* array, u32 size) {
        if (!WriteFloat(buffer, fieldName, array[0])) {
            return false;
        }
        for (u32 i = 1; i < size; i++) {
            if (!WriteFloat(buffer, " ---", array[i])) {
                return false;
            }
        }
        return true;
    }

    bool ReadFloatArray(FileBuffer& buffer, f32* outArray, u32 size) {
        if (!ReadFloat(buffer, outArray)) {
            return false;
        }
        for (u32 i = 1; i < size; i++) {
            if (!ReadFloat(buffer, outArray + i)) {
                return false;
            }
        }
        return true;
    }
};

}  // namespace fs

}  // namespace botwsavs

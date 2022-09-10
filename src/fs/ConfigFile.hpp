#pragma once
#include "File.hpp"
#include "util/NamedValue.hpp"
#include "util/StringBuffer.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
namespace botwsavs {

namespace fs {

class ConfigFile;

class Config {
public:
    virtual void Save(ConfigFile& file) const;
    virtual void Load(ConfigFile& file);
};

class ConfigFile {
public:
    ConfigFile(const char* path) : mFile(path) {}
    ~ConfigFile() = default;

    bool Exists() { return mFile.Exists(); }

    bool Save(const Config& config);
    bool Load(Config& config);

    void WriteInteger(const char* fieldName, const u64 value) {
        if (mSuccess) {
            mSuccess = WriteIntegerInternal(fieldName, value);
        }
    }

    void ReadInteger(u64* outValue) {
        if (mSuccess) {
            mSuccess = ReadIntegerInternal(outValue);
        }
    }

    void WriteString(const char* fieldName, const char* string) {
        if (mSuccess) {
            mSuccess = WriteStringInternal(fieldName, string);
        }
    }

    void ReadString(char* outString, const u32 maxLength) {
        if (mSuccess) {
            mSuccess = ReadStringInternal(outString, maxLength);
        }
    }

    void ReadInteger(u32* outValue) {
        u64 iValue;
        if (!ReadIntegerInternal(&iValue)) {
            mSuccess = false;
            return;
        }

        u32 truncated = (u32)(iValue);
        *outValue = truncated;
    }

    void ReadInteger(s32* outValue) {
        ReadInteger(reinterpret_cast<u32*>(outValue));
    }

    void WriteFloat(const char* fieldName, const f32 value) {
        WriteInteger(fieldName, reinterpret_cast<const u32&>(value));
    }

    void ReadFloat(f32* value) {
        u32 iValue;
        ReadInteger(&iValue);
        if (!mSuccess) {
            return;
        }
        *value = reinterpret_cast<f32&>(iValue);
    }

    template <typename T>
    void WriteIntegerArray(const char* fieldName, const T* array, u32 size) {
        WriteInteger(fieldName, array[0]);
        if (!mSuccess) {
            return;
        }
        for (u32 i = 1; i < size; i++) {
            WriteInteger(" ---", array[i]);
            if (!mSuccess) {
                return;
            }
        }
    }

    template <typename T>
    void ReadIntegerArray(T* outArray, u32 size) {
        ReadInteger(outArray);
        if (!mSuccess) {
            return;
        }
        for (u32 i = 1; i < size; i++) {
            ReadInteger(outArray + i);
            if (!mSuccess) {
                return;
            }
        }
    }

    void WriteFloatArray(const char* fieldName, const f32* array, u32 size) {
        WriteFloat(fieldName, array[0]);
        if (!mSuccess) {
            return;
        }
        for (u32 i = 1; i < size; i++) {
            WriteFloat(" ---", array[i]);
            if (!mSuccess) {
                return;
            }
        }
    }

    void ReadFloatArray(f32* outArray, u32 size) {
        ReadFloat(outArray);
        if (!mSuccess) {
            return;
        }
        for (u32 i = 1; i < size; i++) {
            ReadFloat(outArray + i);
            if (!mSuccess) {
                return;
            }
        }
    }

    template<typename T, u32 L>
    void WriteNamedInteger(const char* fieldName, const util::NamedValue<T, L>& value){
        WriteString("Name of value below", value.GetName());                                   
        WriteInteger(fieldName, value.GetValue());
    }

    template<typename T, u32 L>
    void ReadNamedInteger(util::NamedValue<T, L>& value){
        ReadString(value.Name(), L);                                                        
        ReadInteger(value.GetValuePtr());
    }

    template<typename T, u32 L>
    void WriteNamedFloat(const char* fieldName, const util::NamedValue<T, L>& value){
        WriteString("Name of value below", value.GetName());                                   
        WriteFloat(fieldName, value.GetValue());
    }

    template<typename T, u32 L>
    void ReadNamedFloat(const char* fieldName, const util::NamedValue<T, L>& value){
        ReadString(value.Name(), L);                                                        
        ReadFloat(value.GetValuePtr());
    }

private:
    // In save mode all read functions will be disabled, vise versa
    enum class Mode { Save, Load };

    Mode mMode = Mode::Save;
    bool mSuccess = true;
    File mFile;
    FileBuffer mBuffer;

    bool ReadLine(u32* outLineLength);

    bool WriteIntegerInternal(const char* fieldName, const u64 value);

    bool ReadIntegerInternal(u64* outValue);

    bool WriteStringInternal(const char* fieldName, const char* string);

    bool ReadStringInternal(char* outString, const u32 maxLength);
};

}  // namespace fs

}  // namespace botwsavs
#pragma GCC diagnostic pop

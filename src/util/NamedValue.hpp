#pragma once
#include <prim/seadSafeString.h>
#include <types.h>

namespace botwsavs::util {

template <typename T, u32 L>
class NamedValue {
public:
    NamedValue(T initial) : mValue(initial) { mName[0] = '\0'; }

    void CopyNameFrom(const sead::FixedSafeString<L>& string) {
        strncpy(mName, string.cstr(), L);
        mName[L - 1] = '\0';
    }

    bool NameMatches(const sead::FixedSafeString<L>& string) const {
        return strncmp(mName, string.cstr(), L) == 0;
    }

    void ClearName() { mName[0] = '\0'; }
    const char* GetName() const { return mName; }
    char* Name() { return mName; }
    T GetValue() const { return mValue; }
    T* GetValuePtr() { return &mValue; }
    void SetValue(T value) { mValue = value; }

    void Set(const sead::FixedSafeString<L>& string, T value) {
        CopyNameFrom(string);
        SetValue(value);
    }

private:
    char mName[L];
    T mValue;
};
}  // namespace botwsavs::core

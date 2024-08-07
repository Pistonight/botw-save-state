#pragma once
#include <prim/seadSafeString.h>
#include <exl/types.h>

namespace botwsavs::util {

template <typename T, u32 L>
class NamedValue {
public:
    NamedValue(T initial) : mValue(initial) { mName[0] = '\0'; }

    void set_name(const sead::FixedSafeString<L>& string) {
        strncpy(mName, string.cstr(), L);
        mName[L - 1] = '\0';
    }

    bool name_matches(const sead::FixedSafeString<L>& string) const {
        return strncmp(mName, string.cstr(), L) == 0;
    }

    void clear_name() { mName[0] = '\0'; }
    const char* name() const { return mName; }
    char* name() { return mName; }
    T value() const { return mValue; }
    T* value_ptr() { return &mValue; }
    void SetValue(T value) { mValue = value; }

    void Set(const sead::FixedSafeString<L>& string, T value) {
        set_name(string);
        SetValue(value);
    }

private:
    char mName[L];
    T mValue;
};
}  // namespace botwsavs::util

#pragma once
#include <exl/types.h>
#include <prim/seadSafeString.h>

namespace botw::savs {

template <typename T, u32 L>
class NamedValue {
public:
    NamedValue(T initial) : m_value(initial) { m_name[0] = '\0'; }

    void set_name(const sead::FixedSafeString<L>& string) {
        strncpy(m_name, string.cstr(), L);
        m_name[L - 1] = '\0';
    }

    bool name_matches(const sead::FixedSafeString<L>& string) const {
        return strncmp(m_name, string.cstr(), L) == 0;
    }

    void clear_name() { m_name[0] = '\0'; }
    bool is_name_empty() const { return m_name[0] == '\0'; }
    const char* name() const { return m_name; }
    char* name() { return m_name; }
    T value() const { return m_value; }
    T* value_ptr() { return &m_value; }
    void set_value(T value) { m_value = value; }

    void set(const sead::FixedSafeString<L>& string, T value) {
        set_name(string);
        set_value(value);
    }

private:
    char m_name[L];
    T m_value;
};
}  // namespace botw::savs

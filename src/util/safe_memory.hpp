#pragma once

#include <exl/types.h>

/**
 * Check if the value looks like a pointer
 */
bool ptr_looks_safe(const void* ptr);

namespace botwsavs::util {

class mem_ptr {
public:
    // Create pointer to main
    mem_ptr(void** ptr) { m_ptr = reinterpret_cast<char*>(ptr); }
    // Copy constructor
    mem_ptr(const mem_ptr& other) {
        m_ptr = other.m_ptr;
        m_error = other.m_error;
    }
    // Offset
    mem_ptr& add(s64 offset) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"
        m_ptr += offset;
#pragma GCC diagnostic pop
        return *this;
    }
    // p+x is the same as p.add(xxx)
    mem_ptr& operator+(s64 offset) { return add(offset); }
    // Deferring and storing the value as pointer
    mem_ptr& deref() {
        if (m_error || !ptr_looks_safe(m_ptr)) {
            m_error = true;
            return *this;
        }
        char** pp = reinterpret_cast<char**>(m_ptr);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"
        m_ptr = *pp;
#pragma GCC diagnostic pop
        return *this;
    }
    // p[x] is the same as p.add(xxx).deref()
    mem_ptr& operator[](s64 offset) { return add(offset).deref(); }

public:
    char* m_ptr;
    bool m_error = false;
};

template <typename T>
class safe_ptr {
public:
    safe_ptr(T* ptr) : m_ptr(ptr) {}
    safe_ptr(const safe_ptr& other) : m_ptr(other.m_ptr) {}
    safe_ptr(const mem_ptr& p) {
        m_ptr = p.m_error ? nullptr : reinterpret_cast<T*>(p.m_ptr);
    }

    bool take_ptr(T** out) const {
        if (!looks_safe()) {
            return false;
        }
        *out = m_ptr;
        return true;
    }

    bool get(T* out) const {
        if (!looks_safe()) {
            return false;
        }
        *out = *m_ptr;
        return true;
    }

    bool set(T value) const {
        if (!looks_safe()) {
            return false;
        }
        *m_ptr = value;
        return true;
    }

    bool get(T* out, u32 i) const {
        if (!looks_safe()) {
            return false;
        }
        *out = m_ptr[i];
        return true;
    }

    bool set(T value, u32 i) const {
        if (!looks_safe()) {
            return false;
        }
        m_ptr[i] = value;
        return true;
    }

    bool get_array(T* out, u32 len) const {
        if (!looks_safe()) {
            return false;
        }
        for (u32 i = 0; i < len; i++) {
            out[i] = m_ptr[i];
        }
        return true;
    }

    bool set_array(const T* array, u32 len) const {
        if (!looks_safe()) {
            return false;
        }
        for (u32 i = 0; i < len; i++) {
            m_ptr[i] = array[i];
        }
        return true;
    }

    bool looks_safe() const { 
        return ptr_looks_safe(m_ptr);
    }

private:
    T* m_ptr;
};


}  // namespace botwsavs::mem

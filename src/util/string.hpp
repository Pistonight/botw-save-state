#pragma once

#include <cstring>
#include <stdarg.h>
#include <stdio.h>

#include <exl/types.h>

namespace botw::savs {

template <typename T, u32 L>
class StringBufferBase {
public:
    StringBufferBase() { clear(); }
    void clear() {
        m_len = 0;
        ensure_termination();
    }
    const T* content() const { return m_content; }
    T* content() { return m_content; }
    T* last() { return m_content + m_len; }
    u32 len() const { return m_len; }  // may include null byte in the middle

    void increase_length(u32 size) {
        if (size > 0) {
            m_len += size;
            if (m_len > L) {
                m_len = L;
            }
        }
        ensure_termination();
    }

    void append(const T* text) {
        strncpy(last(), text, L - m_len);
        increase_length(strlen(text));
    }

    void copy(const T* text) {
        clear();
        append(text);
    }

    void ensure_termination() { m_content[m_len] = '\0'; }

    bool index_of(T search, u32 from, u32* outIndex) {
        for (u32 i = from; i < m_len; i++) {
            if (m_content[i] == search) {
                *outIndex = i;
                return true;
            }
        }
        return false;
    }

    void set(u32 i, T c) {
        if (i < m_len) {
            m_content[i] = c;
        }
    }

    void delete_front(u32 size) {
        if (size > m_len) {
            clear();
            return;
        }
        for (u32 i = 0; i < m_len - size; i++) {
            m_content[i] = m_content[i + size];
        }
        m_len -= size;
        ensure_termination();
    }

    void delete_end(u32 size) {
        if (m_len <= size) {
            m_len = 0;
        } else {
            m_len -= size;
        }
        ensure_termination();
    }

protected:
    T m_content[L + 1];
    u32 m_len;
};

template <u32 L>
class StringBuffer : public StringBufferBase<char, L> {
public:
    void appendf(const char* format, ...) {
        va_list args;
        va_start(args, format);
        int size = vsnprintf(this->last(), L - this->m_len, format, args);
        va_end(args);
        this->increase_length(size);
    }
};

template <u32 L>
class WStringBuffer : public StringBufferBase<char16_t, L> {
public:
    void copy_from(const char* text) {
        for (u32 i = 0; i < L; i++) {
            this->m_content[i] = text[i];
            if (text[i] == '\0') {
                this->m_len = i;
                return;
            }
        }
        this->m_len = L;
        this->ensure_termination();
    }
};

}  // namespace botw::savs

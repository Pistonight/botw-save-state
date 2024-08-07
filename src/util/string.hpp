#pragma once

#include <cstring>
#include <stdio.h>

#include <exl/types.h>

namespace botwsavs::util {

template <u32 L>
class StringBuffer {
public:
    StringBuffer() { clear(); }
    void clear() {
        m_len = 0;
        ensure_termination();
    }
    const char* content() const { return m_content; }
    char* last() { return m_content + m_len; }
    u32 len() const { return m_len; } // may include null byte in the middle

    void increase_length(u32 size) {
        if (size > 0) {
            m_len += size;
            if (m_len > L) {
                m_len = L;
            }
            ensure_termination();
        }
    }

    void append(const char* text) {
        strncpy(last(), text, L - m_len);
        increase_length(strlen(text));
    }

    template <typename T>
    void appendf(const char* format, T value) {
        increase_length(sprintf(last(), format, value));
    }

    void ensure_termination() { m_content[m_len] = '\0'; }

    bool index_of(char search, u32 from, u32* outIndex) {
        for (u32 i = from; i < m_len; i++) {
            if (m_content[i] == search) {
                *outIndex = i;
                return true;
            }
        }
        return false;
    }

    void set(u32 i, char c) {
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

private:
    char m_content[L + 1];
    u32 m_len;
};

}  // namespace botwsavs::util

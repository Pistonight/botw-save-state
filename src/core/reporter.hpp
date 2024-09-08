#pragma once

#include <algorithm>
#include <exl/types.h>
#include <toolkit/mem/string.hpp>

namespace botw::savs {

class Reporter {
public:
    Reporter() : m_error_count(0) {}

    void report(const char* field, bool success);

    bool has_error() const { return m_error_count > 0; }
    void mark_error() { m_mark_error = m_error_count; }
    bool has_more_errors() const { return m_mark_error < m_error_count; }
    template <u32 L>
    void append_fields_to(mem::StringBuffer<L>& out) const {
        u32 max = std::min(m_error_count, 5u);
        for (u32 i = 0; i < max; i++) {
            out.appendf("--%s", m_fields[i].content());
            if (i < max - 1) {
                out.append("\n");
            }
        }
    }

private:
    u32 m_error_count = 0;
    u32 m_mark_error = 0;
    mem::StringBuffer<36> m_fields[5];
};

}  // namespace botw::savs

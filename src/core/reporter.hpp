#pragma once

#include <exl/types.h>
#include <util/string.hpp>

namespace botw::savs {

class Reporter {
public:
    Reporter() : m_error_count(0) {}

    void report(const char* field, bool success);

    bool has_error() const { return m_error_count > 0; }

    template<u32 L>
    void get_fields_string(StringBuffer<L>& out) const {
        out.clear();
        switch (m_error_count) {
            case 0:
                break;
            case 1:
                out.append(m_first_field.content());
                break;
            case 2:
                out.append(m_first_field.content());
                out.append(" and ");
                out.append(m_second_field.content());
                break;
            default:
                out.append(m_first_field.content());
                out.append(", ");
                out.append(m_second_field.content());
                out.appendf(" and %d more fields", m_error_count - 2);
        }
    }
private:
    u32 m_error_count;
    StringBuffer<64> m_first_field;
    StringBuffer<64> m_second_field;


};

}

#include "core/reporter.hpp"

namespace botw::savs {

void Reporter::report(const char* field, bool success) {
    if (success) {
        return;
    }
    m_error_count++;
    if (m_error_count > 5) {
        // 1, 2, 3, 4, and X more
        int x_more = m_error_count - 4;
        m_fields[4].clear();
        m_fields[4].appendf("... %d more fields", x_more);
    } else {
        m_fields[m_error_count - 1].copy(field);
    }
    return;
}
} // namespace botw::savs

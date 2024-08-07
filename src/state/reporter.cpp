#include "./reporter.hpp"

namespace botwsavs::state {

void Reporter::report(const char* field, bool success) {
    if (success) {
        return;
    }
    switch (m_error_count) {
        case 0:
            m_first_field.clear();
            m_first_field.append(field);
            break;
        case 1:
            m_second_field.clear();
            m_second_field.append(field);
            break;
    }
    m_error_count++;
}

}

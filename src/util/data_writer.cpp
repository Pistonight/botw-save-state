#include <stdlib.h>

#include "util/data_writer.hpp"

namespace botw::savs {

DataWriter::DataWriter(const char* path) : m_file(path) {
    if (!m_file.exists()) {
        if (!m_file.create()) {
            m_success = false;
            return;
        }
    }
    if (!m_file.open()) {
        m_success = false;
    }
}

bool DataWriter::do_write_number(const char* field_name, u64 value) {
    m_buffer.clear();
    m_buffer.appendf("0x%016x", value);
    m_buffer.appendf("# %s\n", field_name);
    return m_file.write(m_buffer);
}

bool DataWriter::do_write_string(const char* field_name, const char* string) {
    m_buffer.clear();
    m_buffer.appendf("%s", string);
    m_buffer.appendf("# %s\n", field_name);
    return m_file.write(m_buffer);
}

}

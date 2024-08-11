#include <stdlib.h>

#include "util/data_reader.hpp"

namespace botw::savs {

DataReader::DataReader(const char* path) : m_file(path) {
    if (!m_file.open()) {
        m_success = false;
        return;
    }
}

bool DataReader::do_read_line(u32* out_line_length) {
    u32 line_length;
    if (!m_buffer.index_of('\n', 0, &line_length)) {
        // no new line character found, try reading more
        s64 readSize = m_file.read(m_buffer);
        if (readSize < 0) {
            return false;
        }

        if (!m_buffer.index_of('\n', 0, &line_length)) {
            // still no new line, probably error
            return false;
        }
    }

    m_buffer.set(line_length, '\0');
    // find a comment character
    u32 comment_index;
    if (m_buffer.index_of('#', 0, &comment_index)) {
        // remove comment
        m_buffer.set(comment_index, '\0');
    }

    // including the comment length because we need to delete it later
    *out_line_length = line_length;
    return true;
}

bool DataReader::do_read_integer(u64* out_value) {
    u32 line_length;
    if (!do_read_line(&line_length)) {
        return false;
    }

    // Convert to hex
    u64 value = strtol(m_buffer.content(), nullptr, 0);
    m_buffer.delete_front(line_length + 1);
    *out_value = value;
    return true;
}

bool DataReader::do_read_string(char* out_string, const u32 buffer_length) {
    u32 line_length;
    if (!do_read_line(&line_length)) {
        return false;
    }
    strncpy(out_string, m_buffer.content(), buffer_length);
    m_buffer.delete_front(line_length + 1);
    out_string[buffer_length - 1] = '\0';

    return true;
}

}  // namespace botw::savs

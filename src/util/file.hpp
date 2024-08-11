#pragma once
#include <exl/types.h>
#include <nn/fs.h>

#include "util/string.hpp"

namespace botw::savs {

using FileBuffer = StringBuffer<1024>;

class File {
public:
    File(const char* path);
    ~File();

    // Check if file exists
    bool exists();
    // Try creating a new file
    bool create();
    // Open the file for read and write
    bool open();
    // Close the file
    bool close();
    // Set file to empty
    bool clear();
    // Write to file
    bool write(const FileBuffer& buffer);
    // Read into buffer
    s64 read(FileBuffer& buffer);

    const char* path() const { return m_path; }
    bool is_opened() const { return m_open; }

private:
    const char* m_path = nullptr;
    bool m_open = false;
    nn::fs::FileHandle m_handle;
    u64 m_offset = 0;
};

}  // namespace botw::savs

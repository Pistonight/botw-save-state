#pragma once
#include <nn/fs.h>
#include <exl/types.h>

#include "./string.hpp"

namespace botwsavs::util {

using FileBuffer = util::StringBuffer<1024>;

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

    const char* path() const { return mPath; }
    bool is_opened() const { return mOpen; }

private:
    const char* mPath = nullptr;
    bool mOpen = false;
    nn::fs::FileHandle mHandle;
    u64 mOffset = 0;
};

}  // namespace botwsavs

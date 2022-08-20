#pragma once
#include <nn/fs.h>

#include "types.h"
namespace botwsavs {
namespace util {
template <u32 T>
class StringBuffer;
}
namespace fs {
using FileBuffer = util::StringBuffer<1024>;

class File {
public:
    File(const char* path);
    ~File();

    // Check if file exists
    bool Exists();
    // Try creating a new file
    bool Create();
    // Open the file for read and write
    bool Open();
    // Close the file
    bool Close();
    // Set file to empty
    bool Clear();
    // Write to file
    bool Write(const FileBuffer& buffer);
    // Read into buffer
    s64 Read(FileBuffer& buffer);

    const char* Path() const { return mPath; }
    bool isOpened() const { return mOpen; }

private:
    const char* mPath = nullptr;
    bool mOpen = false;
    nn::fs::FileHandle mHandle;
    u64 mOffset = 0;
};

}  // namespace fs
}  // namespace botwsavs

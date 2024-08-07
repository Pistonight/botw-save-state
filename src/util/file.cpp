
#include "./file.hpp"

namespace botwsavs::util {

File::File(const char* path) {
    mPath = path;
}

File::~File() {
    if (mOpen) {
        close();
    }
}

bool File::exists() {
    nn::fs::DirectoryEntryType type;
    nn::Result result = nn::fs::GetEntryType(&type, mPath);

    if (result.IsFailure()) {
        return false;
    }

    return type != nn::fs::DirectoryEntryType_Directory;
}

bool File::create() {
    if (exists()) {
        return false;
    }

    nn::Result result = nn::fs::CreateFile(mPath, 0);

    return result.IsSuccess();
}

bool File::open() {
    if (mOpen) {
        return false;
    }

    if (!exists()) {
        return false;
    }

    nn::Result result =
        nn::fs::OpenFile(&mHandle, mPath, nn::fs::OpenMode_ReadWrite | nn::fs::OpenMode_Append);

    mOpen = result.IsSuccess();
    return mOpen;
}

bool File::close() {
    if (!mOpen) {
        return false;
    }
    nn::fs::CloseFile(mHandle);
    mOpen = false;
    return true;
}

bool File::clear() {
    if (!mOpen) {
        return false;
    }
    nn::Result result = nn::fs::SetFileSize(mHandle, 0);
    mOffset = 0;

    return result.IsSuccess();
}

bool File::write(const FileBuffer& buffer) {
    if (!mOpen) {
        return false;
    }
    nn::Result result = nn::fs::SetFileSize(mHandle, mOffset + buffer.len());
    if (result.IsFailure()) {
        return false;
    }

    result = nn::fs::WriteFile(mHandle, mOffset, buffer.content(), buffer.len(),
                               nn::fs::WriteOption::CreateOption(nn::fs::WriteOptionFlag_Flush));
    if (result.IsFailure()) {
        return false;
    }

    mOffset += buffer.len();

    return true;
}
s64 File::read(FileBuffer& buffer) {
    if (!mOpen) {
        return -1;
    }

    u64 readSize = 0;

    nn::Result result =
        nn::fs::ReadFile(&readSize, mHandle, mOffset, buffer.last(), 1024 - buffer.len());
    if (result.IsFailure()) {
        return -1;
    }

    if (readSize == 0) {
        return -1;
    }

    buffer.increase_length(readSize);
    mOffset += readSize;

    return static_cast<s64>(readSize);
}

}  // namespace botwsavs::fs

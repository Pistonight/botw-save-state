#include "util/StringBuffer.hpp"

#include "File.hpp"

namespace botwsavs::fs {

File::File(const char* path) {
    mPath = path;
}

File::~File() {
    if (mOpen) {
        Close();
    }
}

bool File::Exists() {
    nn::fs::DirectoryEntryType type;
    nn::Result result = nn::fs::GetEntryType(&type, mPath);

    if (result.IsFailure()) {
        return false;
    }

    return type != nn::fs::DirectoryEntryType_Directory;
}

bool File::Create() {
    if (Exists()) {
        return false;
    }

    nn::Result result = nn::fs::CreateFile(mPath, 0);

    return result.IsSuccess();
}

bool File::Open() {
    if (mOpen) {
        return false;
    }

    if (!Exists()) {
        return false;
    }

    nn::Result result =
        nn::fs::OpenFile(&mHandle, mPath, nn::fs::OpenMode_ReadWrite | nn::fs::OpenMode_Append);

    mOpen = result.IsSuccess();
    return mOpen;
}

bool File::Close() {
    if (!mOpen) {
        return false;
    }
    nn::fs::CloseFile(mHandle);
    mOpen = false;
    return true;
}

bool File::Clear() {
    if (!mOpen) {
        return false;
    }
    nn::Result result = nn::fs::SetFileSize(mHandle, 0);
    mOffset = 0;

    return result.IsSuccess();
}

bool File::Write(const FileBuffer& buffer) {
    if (!mOpen) {
        return false;
    }
    nn::Result result = nn::fs::SetFileSize(mHandle, mOffset + buffer.Len());
    if (result.IsFailure()) {
        return false;
    }

    result = nn::fs::WriteFile(mHandle, mOffset, buffer.Content(), buffer.Len(),
                               nn::fs::WriteOption::CreateOption(nn::fs::WriteOptionFlag_Flush));
    if (result.IsFailure()) {
        return false;
    }

    mOffset += buffer.Len();

    return true;
}
s64 File::Read(FileBuffer& buffer) {
    if (!mOpen) {
        return -1;
    }

    u64 readSize = 0;

    nn::Result result =
        nn::fs::ReadFile(&readSize, mHandle, mOffset, buffer.Last(), 1024 - buffer.Len());
    if (result.IsFailure()) {
        return -1;
    }

    if (readSize == 0) {
        return -1;
    }

    buffer.IncLen(readSize);

    return static_cast<s64>(readSize);
}

}  // namespace botwsavs::fs

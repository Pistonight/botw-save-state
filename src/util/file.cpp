
#include "util/file.hpp"

namespace botw::savs {

File::File(const char* path) {
    m_path = path;
}

File::~File() {
    if (m_open) {
        close();
    }
}

bool File::exists() {
    nn::fs::DirectoryEntryType type;
    nn::Result result = nn::fs::GetEntryType(&type, m_path);

    if (result.IsFailure()) {
        return false;
    }

    return type != nn::fs::DirectoryEntryType_Directory;
}

bool File::create() {
    if (exists()) {
        return false;
    }

    nn::Result result = nn::fs::CreateFile(m_path, 0);

    return result.IsSuccess();
}

bool File::open() {
    if (m_open) {
        return false;
    }

    if (!exists()) {
        return false;
    }

    nn::Result result =
        nn::fs::OpenFile(&m_handle, m_path, nn::fs::OpenMode_ReadWrite | nn::fs::OpenMode_Append);

    m_open = result.IsSuccess();
    return m_open;
}

bool File::close() {
    if (!m_open) {
        return false;
    }
    nn::fs::CloseFile(m_handle);
    m_open = false;
    return true;
}

bool File::clear() {
    if (!m_open) {
        return false;
    }
    nn::Result result = nn::fs::SetFileSize(m_handle, 0);
    m_offset = 0;

    return result.IsSuccess();
}

bool File::write(const FileBuffer& buffer) {
    if (!m_open) {
        return false;
    }
    nn::Result result = nn::fs::SetFileSize(m_handle, m_offset + buffer.len());
    if (result.IsFailure()) {
        return false;
    }

    result = nn::fs::WriteFile(m_handle, m_offset, buffer.content(), buffer.len(),
                               nn::fs::WriteOption::CreateOption(nn::fs::WriteOptionFlag_Flush));
    if (result.IsFailure()) {
        return false;
    }

    m_offset += buffer.len();

    return true;
}
s64 File::read(FileBuffer& buffer) {
    if (!m_open) {
        return -1;
    }

    u64 readSize = 0;

    nn::Result result =
        nn::fs::ReadFile(&readSize, m_handle, m_offset, buffer.last(), 1024 - buffer.len());
    if (result.IsFailure()) {
        return -1;
    }

    if (readSize == 0) {
        return -1;
    }

    buffer.increase_length(readSize);
    m_offset += readSize;

    return static_cast<s64>(readSize);
}

}  // namespace botwsavs::fs

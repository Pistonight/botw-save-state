#include "core/Worker.hpp"

#include "util/StringBuffer.hpp"

#include "File.hpp"
#include "SaveFile.hpp"

namespace botwsavs::fs {

bool SaveFile::SaveWorker(const core::Worker& worker) {
    bool result = mFile.Open();
    if (!result) {
        return false;
    }

    result = mFile.Clear();
    if (!result) {
        mFile.Close();
        return false;
    }

    FileBuffer buffer;
    result = result && WriteInteger(buffer, "level", worker.mLevel);

    mFile.Close();
    return result;
}

bool SaveFile::LoadWorker(core::Worker& worker) {
    bool result = mFile.Open();
    if (!result) {
        return false;
    }

    FileBuffer buffer;
    result = ReadInteger(buffer, &worker.mLevel);

    mFile.Close();
    return result;
}

}  // namespace botwsavs::fs

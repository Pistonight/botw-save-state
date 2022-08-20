
#include "Logger.hpp"
#include <nn/fs.h>
#include "util/Time.hpp"

namespace botwsavs::fs {

void Logger::Init() {
    if (mReady) {
        return;
    }
    if (!mFile.Exists()) {
        if (!mFile.Create()) {
            return;
        }
    }

    bool result = mFile.Open();
    if (!result) {
        return;
    }
    result = mFile.Clear();
    if (!result) {
        return;
    }
    mReady = true;
}

void Logger::Close() {
    if (mFile.isOpened()) {
        mFile.Close();
    }
    mReady = false;
}

void Logger::Log(const char* level, const char* message) {
    if (!mReady || mLogging) {
        return;
    }
    mLogging = true;
    FileBuffer buffer;
    buffer.SafeAppend(message);
    mLogging = false;
    Log(level, buffer);
}

void Logger::Log(const char* level, const FileBuffer& content) {
    if (!mReady || mLogging) {
        return;
    }
    mLogging = true;
    mOutputBuffer.Clear();
    util::TimeBuffer time;
    util::CurrentTimeString(time);
    mOutputBuffer.SafeAppendF("[%s]", time.Content());
    mOutputBuffer.SafeAppendF("[%s]", level);
    mOutputBuffer.SafeAppendF("%s\n", content.Content());
    mLogging = false;
    mFile.Write(mOutputBuffer);
}

}  // namespace botwsavs::fs

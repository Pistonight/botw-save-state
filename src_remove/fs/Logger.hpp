#pragma once
#include <nn/fs.h>
#include <string>
#include <types.h>

#include "File.hpp"
#include "util/StringBuffer.hpp"

namespace botwsavs::fs {
// Note that Logger is not thread safe
class Logger {
public:
    static Logger& Instance() {
        static Logger sInstance;
        return sInstance;
    }

private:
    Logger() {}

public:
    Logger(Logger const&) = delete;
    void operator=(Logger const&) = delete;
    ~Logger();

    void Init();
    void Close();

    void Log(const char* level, const char* message);

    void Log(const char* level, const FileBuffer& message);

    template <typename T>
    void LogF(const char* level, const char* format, T value) {
        FileBuffer b;
        b.SafeAppendF(format, value);
        Log(level, b);
    }

    void Info(const char* message) { Log("info", message); }

    void Info(const FileBuffer& message) { Log("info", message); }

    template <typename T>
    void InfoF(const char* format, T value) {
        LogF("info", format, value);
    }

    void Warn(const char* message) { Log("warn", message); }

    void Warn(const FileBuffer& message) { Log("warn", message); }

    template <typename T>
    void WarnF(const char* format, T value) {
        LogF("warn", format, value);
    }

    void Error(const char* message) { Log("error", message); }

    void Error(const FileBuffer& message) { Log("error", message); }

    template <typename T>
    void ErrorF(const char* format, T value) {
        LogF("error", format, value);
    }

    void Debug(const char* message) { Log("debug", message); }

    void Debug(const FileBuffer& message) { Log("debug", message); }

    template <typename T>
    void DebugF(const char* format, T value) {
        LogF("debug", format, value);
    }

private:
    bool mReady = false;
    // prevent logger error being logged and going into infinite recursion
    bool mLogging = false;
    File mFile{"sd:/botwsavs/main.log"};
    FileBuffer mOutputBuffer;
};

}  // namespace botwsavs::fs

#include "main.hpp"
#include "core/WorkerThread.hpp"
#include "fs/File.hpp"
#include "fs/Logger.hpp"

#include "ksys/KSys.hpp"

extern "C" void application_init() {
    nn::fs::MountSdCardForDebug("sd");
    // Create runtime files
#ifdef DEBUG
    botwsavs::fs::File mainLog("sd:/botwsavs/main.log");
    mainLog.Create();

    // Initialize Logger
    botwsavs::fs::Logger::Instance().Init();
#endif
    botwsavs::fs::File latestTxt("sd:/botwsavs/latest.txt");
    latestTxt.Create();

    // Start worker
    botwsavs::core::StartWorkerThread();

    botwsavs::fs::Logger::Instance().LogF("test", "ad%p", botwsavs::ksys::Test);
}

extern "C" void application_clean() {
#ifdef DEBUG
    // Close logger
    botwsavs::fs::Logger::Instance().Close();
#endif
}

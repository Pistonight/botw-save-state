#include "main.hpp"
#include "core/WorkerThread.hpp"
#include "fs/File.hpp"
#include "fs/Logger.hpp"

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
    botwsavs::fs::File workerTxt("sd:/botwsavs/worker.txt");
    workerTxt.Create();

    // Start worker
    botwsavs::core::StartWorkerThread();
}

extern "C" void application_clean() {
#ifdef DEBUG
    // Close logger
    botwsavs::fs::Logger::Instance().Close();
#endif
}

#include "WorkerThread.hpp"
#include <mem.h>
#include <nn/os.h>
#include <nn/time.h>
#include "Worker.hpp"
#include "fs/File.hpp"
#include "fs/Logger.hpp"

namespace botwsavs::core {

static nn::os::ThreadType sThread;

void WorkerMain(void* arg) {
    info("Worker thread started");

    nn::TimeSpan shortWait = nn::TimeSpan::FromNanoSeconds(100000000);  // 3f
    nn::TimeSpan longWait = nn::TimeSpan::FromSeconds(5);

    Worker worker;
    if (!worker.Init()) {
        error("Worker init failed");
    }

    while (true) {
        bool success = worker.Work();

        nn::os::YieldThread();  // let other parts of OS do their thing
        if (!success) {
            nn::os::SleepThread(longWait);
        } else {
            nn::os::SleepThread(shortWait);
        }
    }

    info("Worker thread stopping");
}

void StartWorkerThread() {
    const size_t stackSize = 0x80000;
    void* threadStack = memalign(0x1000, stackSize);

    sThread.thread_name_addr = "botwsavs";
    nn::Result result =
        nn::os::CreateThread(&sThread, WorkerMain, nullptr, threadStack, stackSize, 16, 0);
    if (result.IsFailure()) {
        return;
    }
    nn::os::StartThread(&sThread);
}

}  // namespace botwsavs::core

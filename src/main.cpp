#include <exl/lib.hpp>
#include <nn/fs.h>

#include "util/message.hpp"
#include "./worker.hpp"

extern "C" void exl_main(void* x0, void* x1) {
    exl::hook::Initialize();
    nn::fs::MountSdCardForDebug("sd");

    botwsavs::util::msg::install_hooks();
    
    botwsavs::start_worker_thread();
}

extern "C" NORETURN void exl_exception_entry() {
    EXL_ABORT(0x420);
}

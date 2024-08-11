#include <exl/lib.hpp>
#include <nn/fs.h>

#include "core/worker.hpp"
#include "util/message.hpp"

extern "C" void exl_main(void* x0, void* x1) {
    exl::hook::Initialize();
    nn::fs::MountSdCardForDebug("sd");

    botw::savs::msg::install_hooks();
    botw::savs::start_worker_thread();
}

extern "C" NORETURN void exl_exception_entry() {
    EXL_ABORT(0x420);
}

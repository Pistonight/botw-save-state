#include <exl/lib.hpp>
#include <nn/fs.h>
#include <toolkit/tcp.hpp>
#include <toolkit/equipment.hpp>
#include <toolkit/msg/info.hpp>
#include <toolkit/msg/widget.hpp>

#include "core/worker.hpp"

extern "C" void exl_main(void* x0, void* x1) {
    exl::hook::Initialize();
    nn::fs::MountSdCardForDebug("sd");

    botw::msg::info::init();
    botw::msg::widget::init();
    botw::toolkit::equipment::init();
    botw::tcp::init();

    botw::savs::start_worker_thread();
}

extern "C" NORETURN void exl_exception_entry() {
    EXL_ABORT(0x420);
}

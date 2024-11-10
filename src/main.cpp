#include <megaton/prelude.h>
#include <nn/fs.h>
#include <toolkit/equipment.hpp>
#include <toolkit/msg/info.hpp>
#include <toolkit/msg/widget.hpp>
#include <toolkit/tcp.hpp>

#include "core/worker.hpp"

extern "C" void megaton_main() {
    nn::fs::MountSdCardForDebug("sd");

    botw::msg::info::init();
    botw::msg::widget::init();
    botw::toolkit::equipment::init();
    botw::tcp::init();

    botw::savs::start_worker_thread();
}

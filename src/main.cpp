#include <exl/lib.hpp>
#include <nn/fs.h>

#include "util/message.hpp"
#include "./worker.hpp"

extern "C" void exl_main(void* x0, void* x1) {
    exl::hook::Initialize();
    nn::fs::MountSdCardForDebug("sd");
    
    // patch message system for displaying custom info overlay
    exl::patch::CodePatcher patcher { 0x010D2DC4 };
    patcher.BranchLinkInst(reinterpret_cast<void*>(botwsavs::util::msg::get_message_string_hook));
    
    botwsavs::start_worker_thread();
}

extern "C" NORETURN void exl_exception_entry() {
    EXL_ABORT(0x420);
}

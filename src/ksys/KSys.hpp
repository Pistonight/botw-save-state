#pragma once

#include "types.h"
namespace sead {
struct ControllerMgr;
}

/**
 * MAKE SURE TO UPDATE symbols.ld IF YOU CHANGE SOMETHING HERE
 *
 */
namespace botwsavs::ksys {

// main offset from mod 0x3483000
// 0x0
//_ZN8botwsavs4ksys10MainMemoryE
extern void* MainMemory;
// 0x2CBF658
//_ZN8botwsavs4ksys22sControllerMgrInstanceE
extern sead::ControllerMgr* sControllerMgrInstance;
// 0x1238680    (0000007100A95924) in 1.5.0
//_ZN8botwsavs4ksys25ShowInfoOverlayWithStringEmPKv
extern void* ShowInfoOverlayWithString(u64 idx, const void* p_sead__SafeString);
// 0x123DEA0
// _ZN8botwsavs4ksys16GetMessageStringEPvS1_S1_
extern u64 GetMessageString(void* file, void* messageId, void* outString);
}  // namespace botwsavs::ksys

#pragma once

#include "types.h"
namespace sead {
struct ControllerMgr;
}

/**
 * MAKE SURE TO UPDATE symbols.ld IF YOU CHANGE SOMETHING HERE
 *
 */
namespace botwsavs::mem::KingPtr {

// main offset from mod 0x3483000
// 0x0
// _ZN8botwsavs3mem7KingPtr10MainMemoryE
extern void* MainMemory;

// Other used:
// uking::ui::PauseMenuDataMgr::sInstance
// 0x02CA6D50
// _ZN5uking2ui16PauseMenuDataMgr9sInstanceE
//
}  // namespace botwsavs::mem::KingPtr

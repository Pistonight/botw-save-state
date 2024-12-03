#pragma once

#include <thread/seadCriticalSection.h>

// TODO: upstream to decomp
//
namespace botw::savs {

class RuneMgr {
public:
    static RuneMgr* get_instance();

    void set_current(u32 rune_type);
    u32 get_current();

    char _x0[0x200];
    sead::CriticalSection item_cs;
    u32 rune_type;
};

static_assert(offsetof(RuneMgr, rune_type) == 0x240, "");

} // namespace botw::savs

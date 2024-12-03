#include <toolkit/scoped_lock.hpp>

#include "rune.hpp"

extern "C" {

// 0x02C9A668
extern botw::savs::RuneMgr* botw_savs__RuneMgr;
}

namespace botw::savs {

RuneMgr* RuneMgr::get_instance() { return botw_savs__RuneMgr; }

void RuneMgr::set_current(u32 rune_type) {
    toolkit::ScopedLock lock(&item_cs.mCriticalSectionInner);
    this->rune_type = rune_type;
}

u32 RuneMgr::get_current() {
    toolkit::ScopedLock lock(&item_cs.mCriticalSectionInner);
    return this->rune_type;
}

} // namespace botw::savs

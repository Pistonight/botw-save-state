#define private public
#include <Game/UI/uiPauseMenuDataMgr.h>
#undef private
#include <prim/seadScopedLock.h>

#include "util/scoped_lock.hpp"

namespace uking::ui {

void PauseMenuDataMgr::updateEquippedItemArray() {
    // offset of array: 0x44E70
    mEquippedWeapons.fill({});

    // since sead::CriticalSection is inlined in 1.6
    // our own scoped lock implementation is provided
    botw::savs::ScopedLock lock(&mCritSection.mCriticalSectionInner);
    // const auto lock = sead::makeScopedLock(mCritSection);
    for (auto& item : getItems()) {
        if (item.getType() > PouchItemType::Shield)
            break;
        if (item.isEquipped())
            mEquippedWeapons[u32(item.getType())] = &item;
    }
}

}  // namespace uking::ui

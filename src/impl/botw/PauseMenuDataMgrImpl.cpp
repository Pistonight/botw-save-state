// https://github.com/zeldaret/botw/blob/b3b97a93aafc5618225b64355f610c543c6c7b8e/src/Game/UI/uiPauseMenuDataMgr.cpp
#include <Game/UI/uiPauseMenuDataMgr.h>
#include <prim/seadScopedLock.h>

#include "util/scoped_lock.hpp"

namespace uking::ui {

void PauseMenuDataMgr::updateEquippedItemArray() {
    // offset of array: 0x44E70
    mEquippedWeapons.fill({});

    // since sead::CriticalSection is inlined in 1.6
    // our own scoped lock implementation is provided
    botwsavs::util::ScopedLock lock(&mCritSection.mCriticalSectionInner);
    // const auto lock = sead::makeScopedLock(mCritSection);
    for (auto& item : getItems()) {
        if (item.getType() > PouchItemType::Shield)
            break;
        if (item.isEquipped())
            mEquippedWeapons[u32(item.getType())] = &item;
    }
}

}  // namespace uking::ui

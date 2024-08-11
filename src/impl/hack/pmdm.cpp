
// Need private field access
#define private public
#include <Game/UI/uiPauseMenuDataMgr.h>
#undef private

#include "impl/hack/pmdm.hpp"

namespace botw::savs::hack {

uking::ui::PouchItem* Pmdm::get_equipped_item(uking::ui::PouchItemType t) {
    return m_pmdm->mEquippedWeapons[(int)t];
}

sead::OffsetList<uking::ui::PouchItem>& Pmdm::get_items() {
    return m_pmdm->mItemLists.list1;
}

void PouchItem::set_value(u32 value) {
    m_item->mValue = value;
}

}

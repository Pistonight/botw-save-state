#pragma once

#include <exl/types.h>

namespace sead {
template <typename T>
class OffsetList;
}

namespace uking::ui {
class PauseMenuDataMgr;
class PouchItem;
enum class PouchItemType;
}

namespace botw::savs::hack {

class Pmdm {

public:
    Pmdm(uking::ui::PauseMenuDataMgr* pmdm) : m_pmdm(pmdm) {}

    uking::ui::PouchItem* get_equipped_item(uking::ui::PouchItemType t);
    sead::OffsetList<uking::ui::PouchItem>& get_items();

private:
    uking::ui::PauseMenuDataMgr* m_pmdm;
};

class PouchItem {
public:
    PouchItem(uking::ui::PouchItem* item) : m_item(item) {}

    void set_value(u32 value);

private:
    uking::ui::PouchItem* m_item;
};


}

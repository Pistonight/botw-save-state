#pragma once
#include "util/NamedValue.hpp"
#include "StateStorage.hpp"

namespace uking::ui {
class PouchItem;
}

namespace ksys::act {
class BaseProc;
}
namespace botwsavs::core{


class StateLevel2 : public StateStorage {
public:
    void ReadFromGame() override;
    void WriteToGame() override;
    void ReadFromFile(fs::ConfigFile& file, u32 version) override;
    void WriteToFile(fs::ConfigFile& file) const override;

private:

    void ReadInventoryEquipment(const char* name, uking::ui::PouchItem* pItem, util::NamedValue<u32, 64>& value);
    void WriteInventoryEquipment(const char* name, uking::ui::PouchItem* pItem, const util::NamedValue<u32, 64>& value);
    void ReadOverworldEquipment(const char* name, const mem::SafePtr<ksys::act::BaseProc>& safepActor, const mem::SafePtr<u32>& safepDurability, util::NamedValue<u32, 64>& value);
    void WriteOverworldEquipment(const char* name, const mem::SafePtr<ksys::act::BaseProc>& safepActor, const mem::SafePtr<u32>& safepDurability,const util::NamedValue<u32, 64>& value);
    util::NamedValue<u32, 64> mMenuEquippedArrow{sInvalid};
    util::NamedValue<u32, 64> mMenuEquippedWeapon{sInvalid};
    util::NamedValue<u32, 64> mMenuEquippedBow{sInvalid};
    util::NamedValue<u32, 64> mMenuEquippedShield{sInvalid};
    util::NamedValue<u32, 64> mOverworldEquippedWeapon{sInvalid};
    util::NamedValue<u32, 64> mOverworldEquippedBow{sInvalid};
    util::NamedValue<u32, 64> mOverworldEquippedShield{sInvalid};

};

}

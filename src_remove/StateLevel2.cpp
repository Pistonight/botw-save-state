#include <Game/UI/uiPauseMenuDataMgr.h>
#include <KingSystem/ActorSystem/actBaseProc.h>
#include <prim/seadSafeString.h>

#include "fs/ConfigFile.hpp"
#include "fs/Logger.hpp"
#include "mem/GamePtr.h"

#include "StateLevel2.hpp"
namespace botwsavs::core {

void StateLevel2::ReadInventoryEquipment(const char* name, uking::ui::PouchItem* pItem,
                                         util::NamedValue<u32, 64>& value) {
    if (mem::PtrLooksSafe(pItem)) {
        value.Set(pItem->getName(), pItem->getValue());
    } else {
        value.ClearName();
        SetError(StateError::NotEquipped);
        warnf("Cannot read equipped %s count", name);
    }
}

void StateLevel2::WriteInventoryEquipment(const char* name, uking::ui::PouchItem* pItem,
                                          const util::NamedValue<u32, 64>& value) {
    if (mem::PtrLooksSafe(pItem)) {
        if (value.NameMatches(pItem->getName())) {
            pItem->setValue(value.GetValue());
        } else {
            SetError(StateError::DifferentName);
            warnf("Cannot restore equipped %s count: different name", name);
        }
    } else {
        SetError(StateError::NotEquipped);
        warnf("Cannot restore equipped %s count", name);
    }
}

void StateLevel2::ReadOverworldEquipment(const char* name,
                                         const mem::SafePtr<ksys::act::BaseProc>& safepActor,
                                         const mem::SafePtr<u32>& safepDurability,
                                         util::NamedValue<u32, 64>& value) {
    ksys::act::BaseProc* pActor;
    if (safepActor.TakePtr(&pActor)) {
        u32 durability;
        if (safepDurability.Get(&durability)) {
            value.Set(pActor->getName(), durability);
        } else {
            value.ClearName();
            SetError(StateError::Pointer);
            warnf("Cannot read equipped overworld %s durability", name);
        }
    } else {
        value.ClearName();
        SetError(StateError::NotEquipped);
        warnf("Cannot read equipped overworld %s actor", name);
    }
}

void StateLevel2::WriteOverworldEquipment(const char* name,
                                          const mem::SafePtr<ksys::act::BaseProc>& safepActor,
                                          const mem::SafePtr<u32>& safepDurability,
                                          const util::NamedValue<u32, 64>& value) {
    ksys::act::BaseProc* pActor;
    if (safepActor.TakePtr(&pActor)) {
        if (value.NameMatches(pActor->getName())) {
            if (!safepDurability.Set(value.GetValue())) {
                SetError(StateError::Pointer);
                warnf("Cannot restore equipped overworld %s: pointer error", name);
            }
        } else {
            SetError(StateError::DifferentName);
            warnf("Cannot restore equipped overworld %s: different name", name);
        }
    } else {
        SetError(StateError::NotEquipped);
        warnf("Cannot restore equipped overworld %s actor", name);
    }
}

void StateLevel2::ReadFromGame() {
    uking::ui::PauseMenuDataMgr* pPauseMenuDataMgr;
    if (TakePtrOrError(named(mem::SafePtr(uking::ui::PauseMenuDataMgr::instance())),
                       &pPauseMenuDataMgr)) {
        // update equipped weapons
        pPauseMenuDataMgr->updateEquippedItemArray();

        // Save arrow, weapon, bow, shield
        ReadInventoryEquipment(
            "arrow", pPauseMenuDataMgr->getEquippedWeapon(uking::ui::PouchItemType::Arrow),
            mMenuEquippedArrow);
        ReadInventoryEquipment(
            "weapon", pPauseMenuDataMgr->getEquippedWeapon(uking::ui::PouchItemType::Sword),
            mMenuEquippedWeapon);
        ReadInventoryEquipment("bow",
                               pPauseMenuDataMgr->getEquippedWeapon(uking::ui::PouchItemType::Bow),
                               mMenuEquippedBow);
        ReadInventoryEquipment(
            "shield", pPauseMenuDataMgr->getEquippedWeapon(uking::ui::PouchItemType::Shield),
            mMenuEquippedShield);
    }

    ReadOverworldEquipment("weapon", 
                           mem::GamePtr::OverworldWeaponActor(),
                           mem::GamePtr::OverworldWeaponDurability(), mOverworldEquippedWeapon);
    ReadOverworldEquipment("bow", mem::GamePtr::OverworldBowActor(),
                           mem::GamePtr::OverworldBowDurability(), mOverworldEquippedBow);
    ReadOverworldEquipment("shield", mem::GamePtr::OverworldShieldActor(),
                           mem::GamePtr::OverworldShieldDurability(), mOverworldEquippedShield);
}

void StateLevel2::WriteToGame() {
    uking::ui::PauseMenuDataMgr* pPauseMenuDataMgr;
    if (TakePtrOrError(named(mem::SafePtr(uking::ui::PauseMenuDataMgr::instance())),
                       &pPauseMenuDataMgr)) {
        // update equipped weapons
        pPauseMenuDataMgr->updateEquippedItemArray();

        // Restore arrow, weapon, bow, shield
        WriteInventoryEquipment(
            "arrow", pPauseMenuDataMgr->getEquippedWeapon(uking::ui::PouchItemType::Arrow),
            mMenuEquippedArrow);
        WriteInventoryEquipment(
            "weapon", pPauseMenuDataMgr->getEquippedWeapon(uking::ui::PouchItemType::Sword),
            mMenuEquippedWeapon);
        WriteInventoryEquipment("bow",
                                pPauseMenuDataMgr->getEquippedWeapon(uking::ui::PouchItemType::Bow),
                                mMenuEquippedBow);
        WriteInventoryEquipment(
            "shield", pPauseMenuDataMgr->getEquippedWeapon(uking::ui::PouchItemType::Shield),
            mMenuEquippedShield);
    }

    WriteOverworldEquipment("weapon", mem::GamePtr::OverworldWeaponActor(),
                            mem::GamePtr::OverworldWeaponDurability(), mOverworldEquippedWeapon);
    WriteOverworldEquipment("bow", mem::GamePtr::OverworldBowActor(),
                            mem::GamePtr::OverworldBowDurability(), mOverworldEquippedBow);
    WriteOverworldEquipment("shield", mem::GamePtr::OverworldShieldActor(),
                            mem::GamePtr::OverworldShieldDurability(), mOverworldEquippedShield);
}

void StateLevel2::ReadFromFile(fs::ConfigFile& file, u32 version) {
    if (version < 2) {
        return;
    }
    file.ReadNamedInteger(mMenuEquippedArrow);
    file.ReadNamedInteger(mMenuEquippedWeapon);
    file.ReadNamedInteger(mMenuEquippedBow);
    file.ReadNamedInteger(mMenuEquippedShield);
    file.ReadNamedInteger(mOverworldEquippedWeapon);
    file.ReadNamedInteger(mOverworldEquippedBow);
    file.ReadNamedInteger(mOverworldEquippedShield);
}

void StateLevel2::WriteToFile(fs::ConfigFile& file) const {
    file.WriteNamedInteger(named(mMenuEquippedArrow));
    file.WriteNamedInteger(named(mMenuEquippedWeapon));
    file.WriteNamedInteger(named(mMenuEquippedBow));
    file.WriteNamedInteger(named(mMenuEquippedShield));
    file.WriteNamedInteger(named(mOverworldEquippedWeapon));
    file.WriteNamedInteger(named(mOverworldEquippedBow));
    file.WriteNamedInteger(named(mOverworldEquippedShield));
}

}  // namespace botwsavs::core

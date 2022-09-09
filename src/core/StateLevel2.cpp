#include <Game/UI/uiPauseMenuDataMgr.h>
#include <KingSystem/ActorSystem/actBaseProc.h>
#include <container/seadOffsetList.h>
#include <prim/seadSafeString.h>
#include "State.hpp"
#include "StateMacros.hpp"
#include "fs/Logger.hpp"
#include "mem/GamePtr.h"

namespace botwsavs::core {

void State::ReadLevel2() {
    GameUseSafePtrOrError(uking::ui::PauseMenuDataMgr::instance(), uking::ui::PauseMenuDataMgr,
                          pPauseMenuDataMgr) {
        // test if stuff works
        s32 size = pPauseMenuDataMgr->getItems().size();
        debugf("mCount is %d", size);

        // update equipped weapons
        pPauseMenuDataMgr->updateEquippedItemArray();

        // Save arrow, weapon, bow, shield
        GameUseSafePtr(pPauseMenuDataMgr->getEquippedWeapon(uking::ui::PouchItemType::Arrow),
                       uking::ui::PouchItem, pEquippedArrow) {
            mMenuEquippedArrow.Set(pEquippedArrow->getName(), pEquippedArrow->getValue());
        }
        else {
            mMenuEquippedArrow.ClearName();
            SetError(Error::NotEquipped);
            warn("Cannot read equipped arrow count");
        }

        GameUseSafePtr(pPauseMenuDataMgr->getEquippedWeapon(uking::ui::PouchItemType::Sword),
                       uking::ui::PouchItem, pEquippedWeapon) {
            mMenuEquippedWeapon.Set(pEquippedWeapon->getName(), pEquippedWeapon->getValue());
        }
        else {
            mMenuEquippedWeapon.ClearName();
            SetError(Error::NotEquipped);
            warn("Cannot read equipped weapon");
        }

        GameUseSafePtr(pPauseMenuDataMgr->getEquippedWeapon(uking::ui::PouchItemType::Bow),
                       uking::ui::PouchItem, pEquippedBow) {
            mMenuEquippedBow.Set(pEquippedBow->getName(), pEquippedBow->getValue());
        }
        else {
            mMenuEquippedBow.ClearName();
            SetError(Error::NotEquipped);
            warn("Cannot read equipped bow");
        }

        GameUseSafePtr(pPauseMenuDataMgr->getEquippedWeapon(uking::ui::PouchItemType::Shield),
                       uking::ui::PouchItem, pEquippedShield) {
            mMenuEquippedShield.Set(pEquippedShield->getName(), pEquippedShield->getValue());
        }
        else {
            mMenuEquippedShield.ClearName();
            SetError(Error::NotEquipped);
            warn("Cannot read equipped shield");
        }
    }

    GameUsePtr(mem::GamePtr::OverworldWeaponActor(), ksys::act::BaseProc, pActorWeapon) {
        u32 overworldWeaponDurability;
        GameRead(GamePtr::OverworldWeaponDurability, overworldWeaponDurability) {
            mOverworldEquippedWeapon.Set(pActorWeapon->getName(), overworldWeaponDurability);
        }
        else {
            mOverworldEquippedWeapon.ClearName();
            SetError(Error::Pointer);
            warn("Cannot read equipped overworld weapon durability");
        }
    }
    else {
        mOverworldEquippedWeapon.ClearName();
        SetError(Error::NotEquipped);
        warn("Cannot read equipped overworld weapon actor");
    }

    GameUsePtr(mem::GamePtr::OverworldBowActor(), ksys::act::BaseProc, pActorBow) {
        u32 overworldBowDurability;
        GameRead(GamePtr::OverworldBowDurability, overworldBowDurability) {
            mOverworldEquippedBow.Set(pActorBow->getName(), overworldBowDurability);
        }
        else {
            mOverworldEquippedBow.ClearName();
            SetError(Error::Pointer);
            warn("Cannot read equipped overworld bow durability");
        }
    }
    else {
        mOverworldEquippedBow.ClearName();
        SetError(Error::NotEquipped);
        warn("Cannot read equipped overworld bow actor");
    }

    GameUsePtr(mem::GamePtr::OverworldShieldActor(), ksys::act::BaseProc, pActorShield) {
        u32 overworldShieldDurability;
        GameRead(GamePtr::OverworldShieldDurability, overworldShieldDurability) {
            mOverworldEquippedShield.Set(pActorShield->getName(), overworldShieldDurability);
        }
        else {
            mOverworldEquippedShield.ClearName();
            SetError(Error::Pointer);
            warn("Cannot read equipped overworld shield durability");
        }
    }
    else {
        mOverworldEquippedShield.ClearName();
        SetError(Error::NotEquipped);
        warn("Cannot read equipped overworld shield actor");
    }
}
void State::WriteLevel2() {
    GameUseSafePtr(uking::ui::PauseMenuDataMgr::instance(), uking::ui::PauseMenuDataMgr,
                   pPauseMenuDataMgr) {
        // update equipped weapons
        pPauseMenuDataMgr->updateEquippedItemArray();

        // Restore each of them
        GameUseSafePtr(pPauseMenuDataMgr->getEquippedWeapon(uking::ui::PouchItemType::Arrow),
                       uking::ui::PouchItem, pEquippedArrow) {
            if (mMenuEquippedArrow.NameMatches(pEquippedArrow->getName())) {
                pEquippedArrow->setValue(mMenuEquippedArrow.GetValue());
            } else {
                SetError(Error::DifferentName);
                warn("Cannot restore equipped arrow count: different name");
            }
        }
        else {
            SetError(Error::NotEquipped);
            warn("Cannot restore equipped arrow count");
        }

        GameUseSafePtr(pPauseMenuDataMgr->getEquippedWeapon(uking::ui::PouchItemType::Sword),
                       uking::ui::PouchItem, pEquippedWeapon) {
            if (mMenuEquippedWeapon.NameMatches(pEquippedWeapon->getName())) {
                pEquippedWeapon->setValue(mMenuEquippedWeapon.GetValue());
            } else {
                SetError(Error::DifferentName);
                warn("Cannot restore equipped weapon: different name");
            }
        }
        else {
            SetError(Error::NotEquipped);
            warn("Cannot restore equipped weapon");
        }

        GameUseSafePtr(pPauseMenuDataMgr->getEquippedWeapon(uking::ui::PouchItemType::Bow),
                       uking::ui::PouchItem, pEquippedBow) {
            if (mMenuEquippedBow.NameMatches(pEquippedBow->getName())) {
                pEquippedBow->setValue(mMenuEquippedBow.GetValue());
            } else {
                SetError(Error::DifferentName);
                warn("Cannot restore equipped bow: different name");
            }
        }
        else {
            SetError(Error::NotEquipped);
            warn("Cannot restore equipped bow");
        }

        GameUseSafePtr(pPauseMenuDataMgr->getEquippedWeapon(uking::ui::PouchItemType::Shield),
                       uking::ui::PouchItem, pEquippedShield) {
            if (mMenuEquippedShield.NameMatches(pEquippedShield->getName())) {
                pEquippedShield->setValue(mMenuEquippedShield.GetValue());
            } else {
                SetError(Error::DifferentName);
                warn("Cannot restore equipped shield: different name");
            }
        }
        else {
            SetError(Error::NotEquipped);
            warn("Cannot restore equipped shield");
        }
    }

    GameUsePtr(mem::GamePtr::OverworldWeaponActor(), ksys::act::BaseProc, pActorWeapon) {
        if (mOverworldEquippedWeapon.NameMatches(pActorWeapon->getName())) {
            GameWrite(GamePtr::OverworldWeaponDurability, mOverworldEquippedWeapon.GetValue());
        } else {
            SetError(Error::DifferentName);
            warn("Cannot restore equipped overworld weapon: different name");
        }
    }
    else {
        SetError(Error::NotEquipped);
        warn("Cannot restore equipped overworld weapon actor");
    }

    GameUsePtr(mem::GamePtr::OverworldBowActor(), ksys::act::BaseProc, pActorBow) {
        if (mOverworldEquippedBow.NameMatches(pActorBow->getName())) {
            GameWrite(GamePtr::OverworldBowDurability, mOverworldEquippedBow.GetValue());
        } else {
            SetError(Error::DifferentName);
            warn("Cannot restore equipped overworld bow: different name");
        }
    }
    else {
        SetError(Error::NotEquipped);
        warn("Cannot restore equipped overworld bow actor");
    }

    GameUsePtr(mem::GamePtr::OverworldShieldActor(), ksys::act::BaseProc, pActorShield) {
        if (mOverworldEquippedShield.NameMatches(pActorShield->getName())) {
            GameWrite(GamePtr::OverworldShieldDurability, mOverworldEquippedShield.GetValue());
        } else {
            SetError(Error::DifferentName);
            warn("Cannot restore equipped overworld shield: different name");
        }
    }
    else {
        SetError(Error::NotEquipped);
        warn("Cannot restore equipped overworld shield actor");
    }
}

}  // namespace botwsavs::core

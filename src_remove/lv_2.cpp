#include <KingSystem/ActorSystem/actBaseProcMgr.h>
#include <KingSystem/ActorSystem/actBaseProc.h>

#include "impl/raw_ptr.hpp"
#include "impl/uiPauseMenuDataMgr.h"
#include "core/lv_2.hpp"
#include "core/reporter.hpp"
#include "util/data_reader.hpp"
#include "util/data_writer.hpp"
#include "util/named.h"

namespace botw::savs {

void read_inventory_equipment_durability(
    const char* name,
    const uking::ui::PouchItem* item,
    NamedValue<u32, 64>& value,
    Reporter& r
) {
    if (ptr_looks_safe(item)) {
        value.set(item->getName(), item->getValue());
    } else {
        value.clear_name();
        r.report(name, false);
    }
}

void write_inventory_equipment_durability(
    const char* name,
    uking::ui::PouchItem* item,
    const NamedValue<u32, 64>& value,
    Reporter& r
) {
    if (ptr_looks_safe(item)) {
        if (value.name_matches(item->getName())) {
            item->setValue(value.value());
        }
    } else {
        r.report(name, false);
    }
}

void read_overworld_equipment_durability(
    const char* name,
    const safe_ptr<ksys::act::BaseProc>& actor,
    const safe_ptr<u32>& durability,
    NamedValue<u32, 64>& value,
    Reporter& r
) {
    ksys::act::BaseProc* safe_actor = nullptr;
    // if actor is not found, it's not an error
    // just means the item is not equipped
    if (actor.take_ptr(&safe_actor) && safe_actor) {
        u32 safe_durability;
        if (durability.get(&safe_durability)) {
            value.set(safe_actor->getName(), safe_durability);
            return;
        }
        // if actor is found but not durability, it is an error
        r.report(name, false);
    }
    value.clear_name();
}

void write_overworld_equipment_durability(
    const char* name,
    const safe_ptr<ksys::act::BaseProc>& actor,
    const safe_ptr<u32>& durability,
    const NamedValue<u32, 64>& value,
    Reporter& r
) {
    ksys::act::BaseProc* safe_actor = nullptr;
    // if actor is not found, it's not an error
    // just means the item is not equipped
    if (actor.take_ptr(&safe_actor) && safe_actor) {
        // if name doesn't match, not an error
        // means the equipment was changed
        if (value.name_matches(safe_actor->getName())) {
            if (!durability.set(value.value())) {
                r.report(name, false);
            }
        }
    }
}

void Lv2::read_from_game(Reporter& r) {
    auto* pmdm = uking::ui::PauseMenuDataMgr::instance();
    if (!pmdm) {
        r.report("PMDM", false);
    } else {
        pmdm->updateEquippedItemArray();

        // save arrow, weapon, bow, shield
        auto* arrow = pmdm->mEquippedWeapons[int(uking::ui::PouchItemType::Arrow)];
        read_inventory_equipment_durability(
            "ArrowCnt", 
            arrow, 
            m_menu_equipped_arrow_count, 
            r);

        auto* weapon = pmdm->getEquippedWeapon(uking::ui::PouchItemType::Sword);
        read_inventory_equipment_durability(
            "InvWpnDura",
            weapon,m_menu_equipped_weapon_durability,r
        );

        auto* bow = pmdm->getEquippedWeapon(uking::ui::PouchItemType::Bow);
        read_inventory_equipment_durability(
            "InvBowDura",
            bow,m_menu_equipped_bow_durability,r
        );

        auto* shield = pmdm->getEquippedWeapon(uking::ui::PouchItemType::Shield);
        read_inventory_equipment_durability(
            "InvShdDura",
            shield,m_menu_equipped_shield_durability,r
        );
    }

    read_overworld_equipment_durability(
        "OvWpnDura",
        raw_ptr::overworld_weapon_actor(),
        raw_ptr::overworld_weapon_durability(),
        m_overworld_equipped_weapon_durability,
        r
    );

    read_overworld_equipment_durability(
        "OvBowDura",
        raw_ptr::overworld_bow_actor(),
        raw_ptr::overworld_bow_durability(),
        m_overworld_equipped_bow_durability,
        r
    );

    read_overworld_equipment_durability(
        "OvShdDura",
        raw_ptr::overworld_shield_actor(),
        raw_ptr::overworld_shield_durability(),
        m_overworld_equipped_shield_durability,
        r
    );
}

void Lv2::write_to_game(Reporter& r, bool hold) const {
    auto* pmdm = uking::ui::PauseMenuDataMgr::instance();
    if (!pmdm) {
        r.report("PMDM", false);
    } else {
        pmdm->updateEquippedItemArray();

        write_inventory_equipment_durability(
            "ArrowCnt",
            pmdm->getEquippedWeapon(uking::ui::PouchItemType::Arrow),
            m_menu_equipped_arrow_count,
            r
        );

        write_inventory_equipment_durability(
            "InvWpnDura",
            pmdm->getEquippedWeapon(uking::ui::PouchItemType::Sword),
            m_menu_equipped_weapon_durability,
            r
        );

        write_inventory_equipment_durability(
            "InvBowDura",
            pmdm->getEquippedWeapon(uking::ui::PouchItemType::Bow),
            m_menu_equipped_bow_durability,
            r
        );

        write_inventory_equipment_durability(
            "InvShdDura",
            pmdm->getEquippedWeapon(uking::ui::PouchItemType::Shield),
            m_menu_equipped_shield_durability,
            r
        );
    }

    write_overworld_equipment_durability(
        "OvWpnDura",
        raw_ptr::overworld_weapon_actor(),
        raw_ptr::overworld_weapon_durability(),
        m_overworld_equipped_weapon_durability,
        r
    );

    write_overworld_equipment_durability(
        "OvBowDura",
        raw_ptr::overworld_bow_actor(),
        raw_ptr::overworld_bow_durability(),
        m_overworld_equipped_bow_durability,
        r
    );

    write_overworld_equipment_durability(
        "OvShdDura",
        raw_ptr::overworld_shield_actor(),
        raw_ptr::overworld_shield_durability(),
        m_overworld_equipped_shield_durability,
        r
    );
}

void Lv2::read_from_file(DataReader& r, Version version) {
    if (version < Version::v2) {
        return;
    }

    r.read_named_integer(m_menu_equipped_arrow_count);
    r.read_named_integer(m_menu_equipped_weapon_durability);
    r.read_named_integer(m_menu_equipped_bow_durability);
    r.read_named_integer(m_menu_equipped_shield_durability);
    r.read_named_integer(m_overworld_equipped_weapon_durability);
    r.read_named_integer(m_overworld_equipped_bow_durability);
    r.read_named_integer(m_overworld_equipped_shield_durability);
}

void Lv2::write_to_file(DataWriter& w) const {
    w.write_named_integer(_named(m_menu_equipped_arrow_count));
    w.write_named_integer(_named(m_menu_equipped_weapon_durability));
    w.write_named_integer(_named(m_menu_equipped_bow_durability));
    w.write_named_integer(_named(m_menu_equipped_shield_durability));
    w.write_named_integer(_named(m_overworld_equipped_weapon_durability));
    w.write_named_integer(_named(m_overworld_equipped_bow_durability));
    w.write_named_integer(_named(m_overworld_equipped_shield_durability));
}

}


#include "core/state.hpp"
#include <Game/UI/uiPauseMenuDataMgr.h>
#include <KingSystem/ActorSystem/actBaseProc.h>
#include <KingSystem/ActorSystem/actBaseProcMgr.h>
#include "core/version.hpp"
#include "impl/hack/pmdm.hpp"
#include "impl/hack/sead_list.hpp"
#include "impl/raw_ptr.hpp"
#include "util/data_reader.hpp"
#include "util/data_writer.hpp"
#include "util/named.h"
#include "util/named_value.hpp"

namespace botw::savs {

void StateConfig::save_config(DataWriter& w) const {
    w.write_bool(_named(m_show_restore_message));
    w.write_bool(_named(m_enable_timers));
    w.write_bool(_named(m_enable_overworld_durability));
    w.write_bool(_named(m_enable_inventory));
}

void StateConfig::read_config(DataReader& r) {
    r.read_bool(&m_show_restore_message);
    r.read_bool(&m_enable_timers);
    r.read_bool(&m_enable_overworld_durability);
    r.read_bool(&m_enable_inventory);
}

void read_inventory_equipment_durability(const char* name, const uking::ui::PouchItem* item,
                                         NamedValue<u32, 64>& value, Reporter& r) {
    if (ptr_looks_safe(item)) {
        value.set(item->getName(), item->getValue());
    } else {
        value.clear_name();
        r.report(name, false);
    }
}

void write_inventory_equipment_durability(const char* name, uking::ui::PouchItem* item,
                                          const NamedValue<u32, 64>& value, Reporter& r) {
    if (ptr_looks_safe(item)) {
        if (value.name_matches(item->getName())) {
            hack::PouchItem hack_item(item);
            hack_item.set_value(value.value());
        }
    } else {
        r.report(name, false);
    }
}

void read_overworld_equipment_durability(const char* name,
                                         const safe_ptr<ksys::act::BaseProc>& actor,
                                         const safe_ptr<u32>& durability,
                                         NamedValue<u32, 64>& value, Reporter& r) {
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

void write_overworld_equipment_durability(const char* name,
                                          const safe_ptr<ksys::act::BaseProc>& actor,
                                          const safe_ptr<u32>& durability,
                                          const NamedValue<u32, 64>& value, Reporter& r) {
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

void State::read_from_game(Reporter& r, const StateConfig& config) {
    // essentials
    m_stored_essentials = false;
    r.report("Health", raw_ptr::health().get(&m_health));
    r.report("Stamina", raw_ptr::stamina().get(&m_stamina));
    r.report("HavokPosition", raw_ptr::havok_position().get_array(m_havok_position, 3));
    r.report("MainPositionMatrix",
             raw_ptr::main_position_matrix().get_array(m_main_position_matrix, 12));
    r.report("CamPanMatrix", raw_ptr::camera_pan_matrix().get_array(m_camera_pan_matrix, 12));
    r.report("CamZoom", raw_ptr::camera_zoom().get(&m_camera_zoom));
    r.report("CamTilt", raw_ptr::camera_tilt().get(&m_camera_tilt));
    if (!r.has_error()) {
        m_stored_essentials = true;
    }

    // timers
    r.mark_error();
    m_stored_timers = false;
    if (config.m_enable_timers) {
        r.report("TODPaused", raw_ptr::time_of_day_paused().get(&m_tod_paused));
        r.report("TODUnpause", raw_ptr::time_of_day_unpaused().get(&m_tod_unpaused));
        r.report("BMTimer", raw_ptr::blood_moon().get(&m_blood_moon_timer));
        r.report("TempDmg", raw_ptr::temperature_damage_timer().get(&m_temperature_damage_timer));
        r.report("FlameDmg", raw_ptr::flame_timer().get(&m_flame_timer));
        r.report("Gale", raw_ptr::gale_timer().get(&m_gale_timer));
        r.report("Fury", raw_ptr::fury_timer().get(&m_fury_timer));
        r.report("DarukP", raw_ptr::protection_timer().get(&m_protection_timer));
        r.report("Grace", raw_ptr::grace_timer().get(&m_grace_timer));
        r.report("ChampUse", raw_ptr::ability_uses().get_array(m_ability_uses, 3));
        r.report("MS", raw_ptr::master_sword_cooldown().get(&m_master_sword_cooldown));
        r.report("Speed1", raw_ptr::speed_potion_timer1().get(&m_speed_potion_timer_1));
        r.report("Speed2", raw_ptr::speed_potion_timer2().get(&m_speed_potion_timer_2));
        r.report("Speed3", raw_ptr::speed_potion_timer3().get(&m_speed_potion_timer_3));
        r.report("AtkTime", raw_ptr::attack_potion_timer().get(&m_attack_potion_timer));
        r.report("DefTime", raw_ptr::defense_potion_timer().get(&m_defense_potion_timer));
        r.report("HeatResT", raw_ptr::heat_resist_potion_timer().get(&m_heat_resist_potion_timer));
        r.report("ColdResT", raw_ptr::cold_resist_potion_timer().get(&m_cold_resist_potion_timer));
        r.report("FireResT",
                 raw_ptr::flame_resist_potion_timer().get(&m_flame_resist_potion_timer));
        r.report("ElecResT",
                 raw_ptr::shock_resist_potion_timer().get(&m_shock_resist_potion_timer));
        r.report("StealthT", raw_ptr::stealth_potion_timer().get(&m_stealth_potion_timer));
        if (!r.has_more_errors()) {
            m_stored_timers = true;
        }
    }

    // durability
    r.mark_error();
    m_stored_overworld_durability = false;
    if (config.m_enable_overworld_durability) {
        read_overworld_equipment_durability("OvWpnDura", raw_ptr::overworld_weapon_actor(),
                                            raw_ptr::overworld_weapon_durability(),
                                            m_overworld_equipped_weapon_durability, r);

        read_overworld_equipment_durability("OvBowDura", raw_ptr::overworld_bow_actor(),
                                            raw_ptr::overworld_bow_durability(),
                                            m_overworld_equipped_bow_durability, r);

        read_overworld_equipment_durability("OvShdDura", raw_ptr::overworld_shield_actor(),
                                            raw_ptr::overworld_shield_durability(),
                                            m_overworld_equipped_shield_durability, r);
        if (!r.has_more_errors()) {
            m_stored_overworld_durability = true;
        }
    }

    // inventory
    r.mark_error();
    m_stored_inventory = false;
    if (config.m_enable_inventory) {
        auto* pmdm = uking::ui::PauseMenuDataMgr::instance();
        if (!pmdm) {
            r.report("PMDM", false);
        } else {
            pmdm->updateEquippedItemArray();
            hack::Pmdm hack_pmdm(pmdm);

            // save arrow, weapon, bow, shield
            auto* arrow = hack_pmdm.get_equipped_item(uking::ui::PouchItemType::Arrow);
            read_inventory_equipment_durability("ArrowCnt", arrow, m_menu_equipped_arrow_count, r);

            auto* weapon = hack_pmdm.get_equipped_item(uking::ui::PouchItemType::Sword);
            read_inventory_equipment_durability("InvWpnDura", weapon,
                                                m_menu_equipped_weapon_durability, r);

            auto* bow = hack_pmdm.get_equipped_item(uking::ui::PouchItemType::Bow);
            read_inventory_equipment_durability("InvBowDura", bow, m_menu_equipped_bow_durability,
                                                r);

            auto* shield = hack_pmdm.get_equipped_item(uking::ui::PouchItemType::Shield);
            read_inventory_equipment_durability("InvShdDura", shield,
                                                m_menu_equipped_shield_durability, r);

            auto& items = hack_pmdm.get_items();
            s32 count = items.size();
            s32 real_count = hack::get_sead_list_real_size(&items);
            m_num_inventory_count_offset = real_count - count;
        }
        if (!r.has_more_errors()) {
            m_stored_inventory = true;
        }
    }
}

void State::write_to_game(Reporter& r, const StateConfig& config, bool hold) const {
    // essentials
    if (m_stored_essentials) {
        r.report("Health", raw_ptr::health().set(m_health));
        r.report("Stamina", raw_ptr::stamina().set(m_stamina));
        r.report("HavokPosition", raw_ptr::havok_position().set_array(m_havok_position, 3));
        r.report("MainPositionMatrix",
                 raw_ptr::main_position_matrix().set_array(m_main_position_matrix, 12));
        r.report("CamPanMatrix", raw_ptr::camera_pan_matrix().set_array(m_camera_pan_matrix, 12));
        r.report("CamZoom", raw_ptr::camera_zoom().set(m_camera_zoom));
        r.report("CamTilt", raw_ptr::camera_tilt().set(m_camera_tilt));

        // extras
        r.report("RdBombCD", raw_ptr::round_bomb_cooldown().set(0.0F));
        r.report("SqBombCD", raw_ptr::square_bomb_cooldown().set(0.0F));
        r.report("StasisCD", raw_ptr::stasis_cooldown().set(0.0F));

        auto* proc_mgr = ksys::act::BaseProcMgr::instance();
        if (!proc_mgr) {
            r.report("DelBomb", false);
        } else {
            for (const char* bomb_name : {
                     "RemoteBomb",
                     "RemoteBomb2",
                     "RemoteBombCube",
                     "RemoteBombCube2",
                 }) {
                auto* bomb = proc_mgr->getProc(bomb_name, {});
                if (bomb) {
                    bomb->deleteLater(ksys::act::BaseProc::DeleteReason::_0);
                }
            }
        }
    }

    // timers
    if (m_stored_timers && config.m_enable_timers) {
        r.report("TODPaused", raw_ptr::time_of_day_paused().set(m_tod_paused));
        r.report("TODUnpause", raw_ptr::time_of_day_unpaused().set(m_tod_unpaused));
        r.report("BMTimer", raw_ptr::blood_moon().set(m_blood_moon_timer));
        r.report("TempDmg", raw_ptr::temperature_damage_timer().set(m_temperature_damage_timer));
        r.report("FlameDmg", raw_ptr::flame_timer().set(m_flame_timer));
        r.report("Gale", raw_ptr::gale_timer().set(m_gale_timer));
        r.report("Fury", raw_ptr::fury_timer().set(m_fury_timer));
        r.report("DarukP", raw_ptr::protection_timer().set(m_protection_timer));
        r.report("Grace", raw_ptr::grace_timer().set(m_grace_timer));
        r.report("ChampUse", raw_ptr::ability_uses().set_array(m_ability_uses, 3));
        r.report("MS", raw_ptr::master_sword_cooldown().set(m_master_sword_cooldown));
        r.report("Speed1", raw_ptr::speed_potion_timer1().set(m_speed_potion_timer_1));
        r.report("Speed2", raw_ptr::speed_potion_timer2().set(m_speed_potion_timer_2));
        r.report("Speed3", raw_ptr::speed_potion_timer3().set(m_speed_potion_timer_3));
        r.report("AtkTime", raw_ptr::attack_potion_timer().set(m_attack_potion_timer));
        r.report("DefTime", raw_ptr::defense_potion_timer().set(m_defense_potion_timer));
        r.report("HeatResT", raw_ptr::heat_resist_potion_timer().set(m_heat_resist_potion_timer));
        r.report("ColdResT", raw_ptr::cold_resist_potion_timer().set(m_cold_resist_potion_timer));
        r.report("FireResT", raw_ptr::flame_resist_potion_timer().set(m_flame_resist_potion_timer));
        r.report("ElecResT", raw_ptr::shock_resist_potion_timer().set(m_shock_resist_potion_timer));
        r.report("StealthT", raw_ptr::stealth_potion_timer().set(m_stealth_potion_timer));
    }

    // durability
    if (m_stored_overworld_durability && config.m_enable_overworld_durability) {
        write_overworld_equipment_durability("OvWpnDura", raw_ptr::overworld_weapon_actor(),
                                             raw_ptr::overworld_weapon_durability(),
                                             m_overworld_equipped_weapon_durability, r);

        write_overworld_equipment_durability("OvBowDura", raw_ptr::overworld_bow_actor(),
                                             raw_ptr::overworld_bow_durability(),
                                             m_overworld_equipped_bow_durability, r);

        write_overworld_equipment_durability("OvShdDura", raw_ptr::overworld_shield_actor(),
                                             raw_ptr::overworld_shield_durability(),
                                             m_overworld_equipped_shield_durability, r);
    }

    // inventory
    // Note inventory is only restored while not holding
    if (hold && m_stored_inventory && config.m_enable_inventory) {
        auto* pmdm = uking::ui::PauseMenuDataMgr::instance();
        if (!pmdm) {
            r.report("PMDM", false);
        } else {
            pmdm->updateEquippedItemArray();
            hack::Pmdm hack_pmdm(pmdm);

            write_inventory_equipment_durability(
                "ArrowCnt", hack_pmdm.get_equipped_item(uking::ui::PouchItemType::Arrow),
                m_menu_equipped_arrow_count, r);

            write_inventory_equipment_durability(
                "InvWpnDura", hack_pmdm.get_equipped_item(uking::ui::PouchItemType::Sword),
                m_menu_equipped_weapon_durability, r);

            write_inventory_equipment_durability(
                "InvBowDura", hack_pmdm.get_equipped_item(uking::ui::PouchItemType::Bow),
                m_menu_equipped_bow_durability, r);

            write_inventory_equipment_durability(
                "InvShdDura", hack_pmdm.get_equipped_item(uking::ui::PouchItemType::Shield),
                m_menu_equipped_shield_durability, r);

            auto& items = hack_pmdm.get_items();
            s32 real_size = hack::get_sead_list_real_size(&items);
            s32 size = real_size - m_num_inventory_count_offset;
            hack::set_sead_list_m_count(&items, size);
        }
    }
}

StateFileResult State::read_from_file(DataReader& r) {
    u32 version = 0;
    r.read_integer(&version);
    if (version <= vLegacy) {
        return StateFileResult::UnsupportedVersion;
    }
    if (version > Version::vLatest) {
        return StateFileResult::InvalidVersion;
    }
    r.read_bool(&m_stored_essentials);
    r.read_bool(&m_stored_timers);
    r.read_bool(&m_stored_overworld_durability);
    r.read_bool(&m_stored_inventory);

    r.read_integer(&m_health);
    r.read_float(&m_stamina);
    r.read_float_array(m_havok_position, 3);
    r.read_float_array(m_main_position_matrix, 12);
    r.read_float_array(m_camera_pan_matrix, 12);
    r.read_float(&m_camera_zoom);
    r.read_float(&m_camera_tilt);

    r.read_float(&m_tod_paused);
    r.read_float(&m_tod_unpaused);
    r.read_float(&m_blood_moon_timer);
    r.read_float(&m_temperature_damage_timer);
    r.read_float(&m_flame_timer);
    r.read_float(&m_gale_timer);
    r.read_float(&m_fury_timer);
    r.read_float(&m_protection_timer);
    r.read_float(&m_grace_timer);
    r.read_integer_array(m_ability_uses, 3);
    r.read_float(&m_master_sword_cooldown);
    r.read_float(&m_speed_potion_timer_1);
    r.read_float(&m_speed_potion_timer_2);
    r.read_float(&m_speed_potion_timer_3);
    r.read_float(&m_attack_potion_timer);
    r.read_float(&m_defense_potion_timer);
    r.read_float(&m_heat_resist_potion_timer);
    r.read_float(&m_cold_resist_potion_timer);
    r.read_float(&m_flame_resist_potion_timer);
    r.read_float(&m_shock_resist_potion_timer);
    r.read_float(&m_stealth_potion_timer);

    r.read_named_integer(m_overworld_equipped_weapon_durability);
    r.read_named_integer(m_overworld_equipped_bow_durability);
    r.read_named_integer(m_overworld_equipped_shield_durability);

    r.read_named_integer(m_menu_equipped_arrow_count);
    r.read_named_integer(m_menu_equipped_weapon_durability);
    r.read_named_integer(m_menu_equipped_bow_durability);
    r.read_named_integer(m_menu_equipped_shield_durability);
    r.read_integer(&m_num_inventory_count_offset);
    if (!r.is_successful()) {
        clear();
        return StateFileResult::IOError;
    }
    return StateFileResult::Ok;
}

StateFileResult State::write_to_file(DataWriter& w) const {
    w.write_integer("version", Version::vLatest);

    w.write_bool(_named(m_stored_essentials));
    w.write_bool(_named(m_stored_timers));
    w.write_bool(_named(m_stored_overworld_durability));
    w.write_bool(_named(m_stored_inventory));

    w.write_integer(_named(m_health));
    w.write_float(_named(m_stamina));
    w.write_float_array(_named(m_havok_position), 3);
    w.write_float_array(_named(m_main_position_matrix), 12);
    w.write_float_array(_named(m_camera_pan_matrix), 12);
    w.write_float(_named(m_camera_zoom));
    w.write_float(_named(m_camera_tilt));

    w.write_float(_named(m_tod_paused));
    w.write_float(_named(m_tod_unpaused));
    w.write_float(_named(m_blood_moon_timer));
    w.write_float(_named(m_temperature_damage_timer));
    w.write_float(_named(m_flame_timer));
    w.write_float(_named(m_gale_timer));
    w.write_float(_named(m_fury_timer));
    w.write_float(_named(m_protection_timer));
    w.write_float(_named(m_grace_timer));
    w.write_integer_array(_named(m_ability_uses), 3);
    w.write_float(_named(m_master_sword_cooldown));
    w.write_float(_named(m_speed_potion_timer_1));
    w.write_float(_named(m_speed_potion_timer_2));
    w.write_float(_named(m_speed_potion_timer_3));
    w.write_float(_named(m_attack_potion_timer));
    w.write_float(_named(m_defense_potion_timer));
    w.write_float(_named(m_heat_resist_potion_timer));
    w.write_float(_named(m_cold_resist_potion_timer));
    w.write_float(_named(m_flame_resist_potion_timer));
    w.write_float(_named(m_shock_resist_potion_timer));
    w.write_float(_named(m_stealth_potion_timer));

    w.write_named_integer(_named(m_overworld_equipped_weapon_durability));
    w.write_named_integer(_named(m_overworld_equipped_bow_durability));
    w.write_named_integer(_named(m_overworld_equipped_shield_durability));

    w.write_named_integer(_named(m_menu_equipped_arrow_count));
    w.write_named_integer(_named(m_menu_equipped_weapon_durability));
    w.write_named_integer(_named(m_menu_equipped_bow_durability));
    w.write_named_integer(_named(m_menu_equipped_shield_durability));
    w.write_integer(_named(m_num_inventory_count_offset));

    if (!w.is_successful()) {
        return StateFileResult::IOError;
    }
    return StateFileResult::Ok;
}

}  // namespace botw::savs

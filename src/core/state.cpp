
#include <Game/UI/uiPauseMenuDataMgr.h>
#include <KingSystem/ActorSystem/actBaseProc.h>
#include <KingSystem/ActorSystem/actBaseProcMgr.h>
#include <toolkit/equipment.hpp>
#include <toolkit/io/data_reader.hpp>
#include <toolkit/io/data_writer.hpp>
#include <toolkit/mem/named_value.hpp>
#include <toolkit/pmdm.hpp>
#include <toolkit/sead/list.hpp>
#include <toolkit/tcp.hpp>

#include "core/state.hpp"
#include "core/version.hpp"
#include "impl/raw_ptr.hpp"

namespace botw::savs {

void StateConfig::save_config(io::DataWriter& w) const {
    w.write_bool(_named(m_show_restore_message));
    w.write_bool(_named(m_enable_timers));
    // was overworld durability in v5
    bool unused = false;
    w.write_bool(_named(unused));
    w.write_bool(_named(m_enable_inventory));
    w.write_bool(_named(m_speedometer));
}

void StateConfig::read_config(io::DataReader& r, u32 version) {
    r.read_bool(&m_show_restore_message);
    r.read_bool(&m_enable_timers);
    // was overworld durability in v5
    bool unused;
    r.read_bool(&unused);
    r.read_bool(&m_enable_inventory);

    if (version >= Version::v7) {
        r.read_bool(&m_speedometer);
    }
}

void State::read_from_game(Reporter& r, const StateConfig& config) {
    tcp::sendf("reading state from game\n");
    // essentials
    m_stored_essentials = false;
    r.report("Health", raw_ptr::health().get(&m_health));
    r.report("Stamina", raw_ptr::stamina().get(&m_stamina));
    r.report("HavokPosition",
             raw_ptr::havok_position().get_array(m_havok_position, 3));
    r.report("MainPositionMatrix", raw_ptr::main_position_matrix().get_array(
                                       m_main_position_matrix, 12));
    r.report("CamPanMatrix",
             raw_ptr::camera_pan_matrix().get_array(m_camera_pan_matrix, 12));
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
        r.report("TODUnpause",
                 raw_ptr::time_of_day_unpaused().get(&m_tod_unpaused));
        r.report("BMTimer", raw_ptr::blood_moon().get(&m_blood_moon_timer));
        r.report("TempDmg", raw_ptr::temperature_damage_timer().get(
                                &m_temperature_damage_timer));
        r.report("FlameDmg", raw_ptr::flame_timer().get(&m_flame_timer));
        r.report("Gale", raw_ptr::gale_timer().get(&m_gale_timer));
        r.report("Fury", raw_ptr::fury_timer().get(&m_fury_timer));
        r.report("DarukP",
                 raw_ptr::protection_timer().get(&m_protection_timer));
        r.report("Grace", raw_ptr::grace_timer().get(&m_grace_timer));
        r.report("ChampUse",
                 raw_ptr::ability_uses().get_array(m_ability_uses, 3));
        r.report("MS", raw_ptr::master_sword_cooldown().get(
                           &m_master_sword_cooldown));
        r.report("Speed1",
                 raw_ptr::speed_potion_timer1().get(&m_speed_potion_timer_1));
        r.report("Speed2",
                 raw_ptr::speed_potion_timer2().get(&m_speed_potion_timer_2));
        r.report("Speed3",
                 raw_ptr::speed_potion_timer3().get(&m_speed_potion_timer_3));
        r.report("AtkTime",
                 raw_ptr::attack_potion_timer().get(&m_attack_potion_timer));
        r.report("DefTime",
                 raw_ptr::defense_potion_timer().get(&m_defense_potion_timer));
        r.report("HeatResT", raw_ptr::heat_resist_potion_timer().get(
                                 &m_heat_resist_potion_timer));
        r.report("ColdResT", raw_ptr::cold_resist_potion_timer().get(
                                 &m_cold_resist_potion_timer));
        r.report("FireResT", raw_ptr::flame_resist_potion_timer().get(
                                 &m_flame_resist_potion_timer));
        r.report("ElecResT", raw_ptr::shock_resist_potion_timer().get(
                                 &m_shock_resist_potion_timer));
        r.report("StealthT",
                 raw_ptr::stealth_potion_timer().get(&m_stealth_potion_timer));
        if (!r.has_more_errors()) {
            m_stored_timers = true;
        }
    }

    // inventory
    r.mark_error();
    m_stored_inventory = false;
    if (config.m_enable_inventory) {
        toolkit::PmdmAccess pmdm;
        if (pmdm.is_nullptr()) {
            r.report("PMDM", false);
        } else {
            r.report("PmdmState", m_pmdm_state.read_from(pmdm));
            m_num_inventory_count_offset = pmdm.get_offset_slots();
        }
        if (!r.has_more_errors()) {
            m_stored_inventory = true;
        }
    }
}

void State::write_to_game(Reporter& r, const StateConfig& config,
                          bool hold) const {
    // essentials
    if (m_stored_essentials) {
        r.report("Health", raw_ptr::health().set(m_health));
        r.report("Stamina", raw_ptr::stamina().set(m_stamina));
        r.report("HavokPosition",
                 raw_ptr::havok_position().set_array(m_havok_position, 3));
        r.report("MainPositionMatrix",
                 raw_ptr::main_position_matrix().set_array(
                     m_main_position_matrix, 12));
        r.report("CamPanMatrix", raw_ptr::camera_pan_matrix().set_array(
                                     m_camera_pan_matrix, 12));
        r.report("CamZoom", raw_ptr::camera_zoom().set(m_camera_zoom));
        r.report("CamTilt", raw_ptr::camera_tilt().set(m_camera_tilt));

        // extras
        r.report("RdBombCD", raw_ptr::round_bomb_cooldown().set(360.0F));
        r.report("SqBombCD", raw_ptr::square_bomb_cooldown().set(360.0F));
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
        r.report("TODUnpause",
                 raw_ptr::time_of_day_unpaused().set(m_tod_unpaused));
        r.report("BMTimer", raw_ptr::blood_moon().set(m_blood_moon_timer));
        r.report("TempDmg", raw_ptr::temperature_damage_timer().set(
                                m_temperature_damage_timer));
        r.report("FlameDmg", raw_ptr::flame_timer().set(m_flame_timer));
        r.report("Gale", raw_ptr::gale_timer().set(m_gale_timer));
        r.report("Fury", raw_ptr::fury_timer().set(m_fury_timer));
        r.report("DarukP", raw_ptr::protection_timer().set(m_protection_timer));
        r.report("Grace", raw_ptr::grace_timer().set(m_grace_timer));
        r.report("ChampUse",
                 raw_ptr::ability_uses().set_array(m_ability_uses, 3));
        r.report("MS",
                 raw_ptr::master_sword_cooldown().set(m_master_sword_cooldown));
        r.report("Speed1",
                 raw_ptr::speed_potion_timer1().set(m_speed_potion_timer_1));
        r.report("Speed2",
                 raw_ptr::speed_potion_timer2().set(m_speed_potion_timer_2));
        r.report("Speed3",
                 raw_ptr::speed_potion_timer3().set(m_speed_potion_timer_3));
        r.report("AtkTime",
                 raw_ptr::attack_potion_timer().set(m_attack_potion_timer));
        r.report("DefTime",
                 raw_ptr::defense_potion_timer().set(m_defense_potion_timer));
        r.report("HeatResT", raw_ptr::heat_resist_potion_timer().set(
                                 m_heat_resist_potion_timer));
        r.report("ColdResT", raw_ptr::cold_resist_potion_timer().set(
                                 m_cold_resist_potion_timer));
        r.report("FireResT", raw_ptr::flame_resist_potion_timer().set(
                                 m_flame_resist_potion_timer));
        r.report("ElecResT", raw_ptr::shock_resist_potion_timer().set(
                                 m_shock_resist_potion_timer));
        r.report("StealthT",
                 raw_ptr::stealth_potion_timer().set(m_stealth_potion_timer));
    }

    // inventory
    if (!hold && m_stored_inventory && config.m_enable_inventory) {
        toolkit::PmdmAccess pmdm;
        if (pmdm.is_nullptr()) {
            r.report("PMDM", false);
        } else {
            m_pmdm_state.write_to(pmdm, true);
            pmdm.set_offset_slots(m_num_inventory_count_offset);
            int status = toolkit::equipment::sync_with_pmdm();
            if (status == -1) {
                r.report("SyncPMDM", false);
            } else if (status == 1) {
                r.report("SyncNotPaused", false);
            }
        }
    }
}

StateFileResult State::read_from_file(io::DataReader& r) {
    u32 version = 0;
    r.read_integer(&version);
    if (version <= Version::vLegacy) {
        return StateFileResult::UnsupportedVersion;
    }
    if (version > Version::vLatest) {
        return StateFileResult::InvalidVersion;
    }
    r.read_bool(&m_stored_essentials);
    r.read_bool(&m_stored_timers);
    // was overworld durability in v5
    bool unused;
    r.read_bool(&unused);
    r.read_bool(&m_stored_inventory);
    if (version < Version::v6) {
        // due to incompatibility with inventory state in v5, turn it off
        m_stored_inventory = false;
    }

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

    if (version >= Version::v6) {
        // read inventory
        r.read_integer(&m_num_inventory_count_offset);
        m_pmdm_state.read_from_file(r);
    }

    if (!r.is_successful()) {
        clear();
        return StateFileResult::IOError;
    }
    return StateFileResult::Ok;
}

StateFileResult State::write_to_file(io::DataWriter& w) const {
    tcp::sendf("writing state to file - version %d\n", Version::vLatest);
    w.write_integer("version", Version::vLatest);

    tcp::sendf("-- essentials\n");
    w.write_bool(_named(m_stored_essentials));
    w.write_bool(_named(m_stored_timers));
    // was overworld durability in v5
    bool unused = false;
    w.write_bool(_named(unused));
    w.write_bool(_named(m_stored_inventory));

    w.write_integer(_named(m_health));
    w.write_float(_named(m_stamina));
    w.write_float_array(_named(m_havok_position), 3);
    w.write_float_array(_named(m_main_position_matrix), 12);
    w.write_float_array(_named(m_camera_pan_matrix), 12);
    w.write_float(_named(m_camera_zoom));
    w.write_float(_named(m_camera_tilt));

    tcp::sendf("-- timers\n");
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

    tcp::sendf("-- inventory\n");
    w.write_integer(_named(m_num_inventory_count_offset));
    m_pmdm_state.write_to_file(w);

    if (!w.is_successful()) {
        return StateFileResult::IOError;
    }
    return StateFileResult::Ok;
}

} // namespace botw::savs

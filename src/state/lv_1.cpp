#include "raw_ptr.hpp"
#include "named.h"
#include "util/data_reader.hpp"
#include "util/data_writer.hpp"

#include "./lv_1.hpp"
#include "./reporter.hpp"


namespace botwsavs::state {

void Lv1::read_from_game(Reporter& r) {
    r.report("Health", raw_ptr::health().get(&m_health));
    r.report("Stamina", raw_ptr::stamina().get(&m_stamina));
    r.report("HavokPosition", raw_ptr::havok_position().get_array(m_havok_position, 3));
    r.report("MainPositionMatrix", raw_ptr::main_position_matrix().get_array(m_main_position_matrix, 12));
    r.report("CamPanMatrix", raw_ptr::camera_pan_matrix().get_array(m_camera_pan_matrix, 12));
    r.report("CamZoom", raw_ptr::camera_zoom().get(&m_camera_zoom));
    r.report("CamTilt", raw_ptr::camera_tilt().get(&m_camera_tilt));
}

void Lv1::write_to_game(Reporter& r, bool hold) const {
    r.report("Health", raw_ptr::health().set(m_health));
    r.report("Stamina", raw_ptr::stamina().set(m_stamina));
    r.report("HavokPosition", raw_ptr::havok_position().set_array(m_havok_position, 3));
    r.report("MainPositionMatrix", raw_ptr::main_position_matrix().set_array(m_main_position_matrix, 12));
    r.report("CamPanMatrix", raw_ptr::camera_pan_matrix().set_array(m_camera_pan_matrix, 12));
    r.report("CamZoom", raw_ptr::camera_zoom().set(m_camera_zoom));
    r.report("CamTilt", raw_ptr::camera_tilt().set(m_camera_tilt));
    
    // extras
    r.report("RdBombCD", raw_ptr::round_bomb_cooldown().set(0.0F));
    r.report("SqBombCD", raw_ptr::square_bomb_cooldown().set(0.0F));
    r.report("StasisCD", raw_ptr::stasis_cooldown().set(0.0F));
    // TODO: despawn bombs
    // TODO: fall damage cancel
}

void Lv1::read_from_file(util::DataReader& r, Version version) {
    if (version < Version::v1) {
        return;
    }
    r.read_integer(&m_health);
    r.read_float(&m_stamina);
    r.read_float_array(m_havok_position, 3);
    r.read_float_array(m_main_position_matrix, 12);
    r.read_float_array(m_camera_pan_matrix, 12);
    r.read_float(&m_camera_zoom);
    r.read_float(&m_camera_tilt);
}

void Lv1::write_to_file(util::DataWriter& w) const {
    w.write_integer(_named(m_health));
    w.write_float(_named(m_stamina));
    w.write_float_array(_named(m_havok_position), 3);
    w.write_float_array(_named(m_main_position_matrix), 12);
    w.write_float_array(_named(m_camera_pan_matrix), 12);
    w.write_float(_named(m_camera_zoom));
    w.write_float(_named(m_camera_tilt));
}

}  // namespace botwsavs::core

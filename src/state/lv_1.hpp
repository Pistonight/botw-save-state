#pragma once

#include <exl/types.h>
#include "./version.hpp"

namespace botwsavs::util {
class DataReader;
class DataWriter;
}


namespace botwsavs::state {

class Reporter;

class Lv1 {
public:
    void read_from_game(Reporter& reporter);
    void write_to_game(Reporter& reporter, bool hold) const;
    void read_from_file(util::DataReader& reader, Version version);
    void write_to_file(util::DataWriter& writer) const;

private:
    u32 m_health;
    float m_stamina;
    float m_havok_position[3];
    float m_main_position_matrix[12];
    float m_camera_pan_matrix[12];
    float m_camera_zoom;
    float m_camera_tilt;
    // Extras:
    // reset rune cooldown
    // TODO reset fall damage
};

}  // namespace botwsavs::core

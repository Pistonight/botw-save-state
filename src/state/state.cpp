#include "./state.hpp"

namespace botwsavs::state {

void State::read_from_game(Reporter& reporter, u32 level) {
    switch (level) {
        case 3:
            /* mStorageLevel3.ReadFromGame(); */
        // fall through
        case 2:
            m_lv_2.read_from_game(reporter);
        // fall through
        case 1:
            m_lv_1.read_from_game(reporter);
    }
    m_level = level;
}

void State::write_to_game(Reporter& reporter, u32 level, bool hold) const {
    switch (std::min(level, m_level)) {
        case 3:
            /* mStorageLevel3.WriteToGame(); */
        // fall through
        case 2:
            m_lv_2.write_to_game(reporter, hold);
        // fall through
        case 1:
            m_lv_1.write_to_game(reporter, hold);
    }
}

void State::read_from_file(util::DataReader& reader) {
    u32 version = 0;
    reader.read_integer(&version);
    reader.read_integer(&m_level);

    if (version < 1 || version > Version::vLatest) {
        return;
    }

    m_lv_1.read_from_file(reader, static_cast<Version>(version));
    m_lv_2.read_from_file(reader, static_cast<Version>(version));
}

void State::write_to_file(util::DataWriter& writer) const {
    writer.write_integer("version", Version::vLatest);
    writer.write_integer("level", m_level);

    m_lv_1.write_to_file(writer);
    m_lv_2.write_to_file(writer);
}

}

#pragma once

#include <exl/types.h>
#include "state/version.hpp"
#include "util/named_value.hpp"

namespace botwsavs::util {
class DataReader;
class DataWriter;
}

namespace botwsavs::state {

class Reporter;

class Lv2 {
public:
    void read_from_game(Reporter& reporter);
    void write_to_game(Reporter& reporter, bool hold) const;
    void read_from_file(util::DataReader& reader, Version version);
    void write_to_file(util::DataWriter& writer) const;

private:
    util::NamedValue<u32, 64> m_menu_equipped_arrow_count{0};
    util::NamedValue<u32, 64> m_menu_equipped_weapon_durability{0};
    util::NamedValue<u32, 64> m_menu_equipped_bow_durability{0};
    util::NamedValue<u32, 64> m_menu_equipped_shield_durability{0};
    util::NamedValue<u32, 64> m_overworld_equipped_weapon_durability{0};
    util::NamedValue<u32, 64> m_overworld_equipped_bow_durability{0};
    util::NamedValue<u32, 64> m_overworld_equipped_shield_durability{0};
};

}  // namespace botwsavs::core

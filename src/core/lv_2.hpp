#pragma once

#include <exl/types.h>
#include "core/version.hpp"
#include "util/named_value.hpp"

namespace botw::savs {

class DataReader;
class DataWriter;
class Reporter;

class Lv2 {
public:
    void read_from_game(Reporter& reporter);
    void write_to_game(Reporter& reporter, bool hold) const;
    void read_from_file(DataReader& reader, Version version);
    void write_to_file(DataWriter& writer) const;

private:
    NamedValue<u32, 64> m_menu_equipped_arrow_count{0};
    NamedValue<u32, 64> m_menu_equipped_weapon_durability{0};
    NamedValue<u32, 64> m_menu_equipped_bow_durability{0};
    NamedValue<u32, 64> m_menu_equipped_shield_durability{0};
    NamedValue<u32, 64> m_overworld_equipped_weapon_durability{0};
    NamedValue<u32, 64> m_overworld_equipped_bow_durability{0};
    NamedValue<u32, 64> m_overworld_equipped_shield_durability{0};
};

}  // namespace botwsavs::core

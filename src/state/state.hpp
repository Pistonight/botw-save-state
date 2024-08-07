#pragma once

#include <exl/types.h>

#include "util/data_reader.hpp"
#include "util/data_writer.hpp"

#include "./lv_1.hpp"
#include "./reporter.hpp"

namespace botwsavs::state {

class State {
public:
    void read_from_game(Reporter& reporter, u32 level);
    void write_to_game(Reporter& reporter, u32 level, bool hold) const;
    void read_from_file(util::DataReader& reader);
    void write_to_file(util::DataWriter& writer) const;

    u32 get_level() const {
        return m_level;
    }
private:
    u32 m_level = 0;
    Lv1 m_lv_1;
};

}

#pragma once

#include <exl/types.h>

#include "state/lv_1.hpp"
#include "state/lv_2.hpp"
#include "state/reporter.hpp"
#include "util/data_reader.hpp"
#include "util/data_writer.hpp"

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
    Lv2 m_lv_2;
};

}

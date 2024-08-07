#pragma once

#include <exl/types.h>
#include "state/state.hpp"
#include "./controller.hpp"

namespace botwsavs {

constexpr const char* OPTION_FILE_PATH = "sd:/botwsavs/option.txt";
constexpr const char* STATE_FILE_PATH = "sd:/botwsavs/latest.txt";

void worker_main(void*);

void start_worker_thread();

class Worker {
public:

    bool initialize() {
        if (!m_controller.initialize()) {
            return false;
        }
        load_options();
        return true;
    }

    void do_work();

    void save_options() const;
    void load_options();

    void execute_save();
    void execute_restore();
    void execute_save_file();
    void execute_restore_file();

    state::State& get_last_restored_state() {
        if (m_last_restored_is_from_memory) {
            return m_memory_state;
        }
        return m_last_restored_file;
    }

private:
    Controller m_controller;
    state::State m_memory_state;
    state::State m_last_restored_file;
    u32 m_active_level = 1;
    bool m_show_restore_message = true;
    bool m_last_restored_is_from_memory = false;
    bool m_last_restored_ok = false;

};

}

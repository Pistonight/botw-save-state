#pragma once

#include <exl/types.h>
#include "core/controller.hpp"
#include "core/state.hpp"

namespace botw::savs {

constexpr const char* OPTION_FILE_PATH = "sd:/botwsavs/option.txt";
constexpr const char* STATE_FILE_PATH = "sd:/botwsavs/latest.txt";
constexpr const char* RESTORE_FILE_PATH = "sd:/botwsavs/restore.txt";

void worker_main(void*);

void start_worker_thread();

class Worker {
public:
    Worker() { m_controller.m_config = &m_config; }

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

    State& get_last_restored_state() {
        if (m_last_restored_is_from_memory) {
            return m_memory_state;
        }
        return m_last_restored_file;
    }

private:
    Controller m_controller;
    bool m_showed_welcome = false;
    float m_player_pos[3]{0, 0, 0};
    u32 m_pos_diff_ticks = 0;

    State m_memory_state;
    State m_last_restored_file;
    bool m_last_restored_is_from_memory = false;
    bool m_last_restored_ok = false;

    StateConfig m_config;

    bool show_active_mode_message() const;
    void welcome();
};

}  // namespace botw::savs

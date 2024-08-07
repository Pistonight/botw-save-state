#include <nn/os.h>
#include <nn/time.h>

#include "mem.h"
#include "named.h"
#include "state/reporter.hpp"
#include "util/data_reader.hpp"
#include "util/data_writer.hpp"
#include "util/message.hpp"

#include "./worker.hpp"

namespace botwsavs {

static nn::os::ThreadType s_thread;

void worker_main(void*) {
    Worker worker;
    while (true) {
        if (worker.initialize()) {
            break;
        }
        nn::os::YieldThread();
        nn::os::SleepThread(nn::TimeSpan::FromSeconds(5));
    }
    while (true) {
        worker.do_work();
        nn::os::YieldThread();
        nn::os::SleepThread(nn::TimeSpan::FromNanoSeconds(100000000));
    }
}

void start_worker_thread() {
    const u64 STACK_SIZE = 0x80000;
    void* thread_stack= memalign(0x1000, STACK_SIZE);

    nn::Result result =
        nn::os::CreateThread(
        &s_thread, 
        worker_main, nullptr, thread_stack, STACK_SIZE, 16);
    if (result.IsFailure()) {
        return;
    }
    nn::os::StartThread(&s_thread);
}

void Worker::do_work() {
    Command command = m_controller.update();
    switch (command) {
        case Command::Save:
            execute_save();
            break;
        case Command::SaveFile:
            execute_save_file();
            break;
        case Command::Restore:
            execute_restore();
            break;
        case Command::RestoreFile:
            execute_restore_file();
            break;
        case Command::PostRestoreHold:
            if (m_last_restored_ok) {
                state::Reporter reporter;
                get_last_restored_state().write_to_game(reporter, m_active_level, true);
            }
            break;
        case Command::RestoreDone:
            if (m_last_restored_ok) {
                if (m_show_restore_message) {
                    if (m_last_restored_is_from_memory) {
                        util::msg::show_customf("Restored state from memory (level %d)", m_active_level);
                    } else {
                        util::msg::show_customf("Restored state from file (level %d)", m_active_level);
                    }
                }
                m_last_restored_ok = false;
            }
            break;
        case Command::SwitchMode:
            if (m_controller.get_mode() == Controller::Mode::Active) {
                util::msg::show_custom("Save State: Active Mode");
            } else {
                util::msg::show_custom("Save State: Setting Mode");
            }
            break;
        case Command::IncreaseLevel:
            if (m_active_level < 3) {
                m_active_level++;
                save_options();
            }
            util::msg::show_set_level(m_active_level);
            break;
        case Command::DecreaseLevel:
            if (m_active_level > 0) {
                m_active_level--;
                save_options();
            }
            util::msg::show_set_level(m_active_level);
            break;
        case Command::SaveOption:
            save_options();
            break;
        case Command::None:
            break;
    }
}

void Worker::save_options() const {
    util::DataWriter writer(OPTION_FILE_PATH);

    writer.write_integer(_named(m_active_level));
    u32 show_restore_message = m_show_restore_message ? 1 : 0;
    writer.write_integer(_named(show_restore_message));

    m_controller.save_key_bindings(writer);
}

void Worker::load_options() {
    util::DataReader reader(OPTION_FILE_PATH);

    u32 active_level = 1;
    reader.read_integer(&active_level);
    u32 show_restore_message = 1;
    reader.read_integer(&show_restore_message);

    if (reader.is_successful()) {
        m_active_level = active_level;
        m_show_restore_message = show_restore_message != 0;
    } else {
        return;
    }

    m_controller.load_key_bindings(reader);

}

void Worker::execute_save() {
    if (m_active_level == 0) {
        return;
    }

    state::Reporter reporter;
    m_memory_state.read_from_game(reporter, m_active_level);
    if (reporter.has_error()) {
        util::StringBuffer<160> fields;
        reporter.get_fields_string(fields);
        util::msg::show_customf("Failed to access %s!", fields.content());
        return;
    }

    util::msg::show_customf("Saved state to memory (level %d)", m_active_level);
}

void Worker::execute_save_file() {
    if (m_active_level == 0) {
        return;
    }

    state::State temp_state;
    state::Reporter reporter;
    temp_state.read_from_game(reporter, m_active_level);
    if (reporter.has_error()) {
        util::StringBuffer<160> fields;
        reporter.get_fields_string(fields);
        util::msg::show_customf("Failed to access %s!", fields.content());
        return;
    }

    util::DataWriter writer(STATE_FILE_PATH);
    temp_state.write_to_file(writer);
    if (writer.is_successful()) {
        util::msg::show_customf("Saved state to file (level %d)", m_active_level);
    } else {
        util::msg::show_custom("Failed to save state: file operation failed!");
    }
}

void Worker::execute_restore() {
    m_last_restored_ok = false;
    if (m_active_level == 0) {
        return;
    }
    if (m_memory_state.get_level() == 0) {
        util::msg::show_custom("No state saved in memory yet!");
        return;
    }

    state::Reporter reporter;
    m_memory_state.write_to_game(reporter, m_active_level, false);
    if (reporter.has_error()) {
        util::StringBuffer<160> fields;
        reporter.get_fields_string(fields);
        util::msg::show_customf("Failed to write %s!", fields.content());
        return;
    }
    m_last_restored_ok = true;
    m_last_restored_is_from_memory = true;
}

void Worker::execute_restore_file() {
    m_last_restored_ok = false;
    if (m_active_level == 0) {
        return;
    }

    util::File file(STATE_FILE_PATH);
    if (!file.exists()) {
        util::msg::show_custom("No restore.txt found!");
        return;
    }

    util::DataReader reader(STATE_FILE_PATH);
    m_last_restored_file.read_from_file(reader);
    if (!reader.is_successful()) {
        util::msg::show_custom("Error reading restore.txt! The file could be corrupted.");
        return;
    }

    state::Reporter reporter;
    m_last_restored_file.write_to_game(reporter, m_active_level, false);
    if (reporter.has_error()) {
        util::StringBuffer<160> fields;
        reporter.get_fields_string(fields);
        util::msg::show_customf("Failed to write %s!", fields.content());
        return;
    }

    m_last_restored_ok = true;
    m_last_restored_is_from_memory = false;
}

}

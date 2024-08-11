#include <Game/UI/uiPauseMenuDataMgr.h>
#include <KingSystem/ActorSystem/actBaseProcMgr.h>
#include <cstdlib>
#include <nn/os.h>
#include <nn/time.h>

#include "core/reporter.hpp"
#include "core/version.hpp"
#include "core/worker.hpp"
#include "impl/raw_ptr.hpp"
#include "util/data_reader.hpp"
#include "util/data_writer.hpp"
#include "util/mem.h"
#include "util/message.hpp"

namespace botw::savs {

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
    void* thread_stack = memalign(0x1000, STACK_SIZE);

    nn::Result result =
        nn::os::CreateThread(&s_thread, worker_main, nullptr, thread_stack, STACK_SIZE, 16);
    if (result.IsFailure()) {
        return;
    }
    nn::os::StartThread(&s_thread);
}

void Worker::do_work() {
    welcome();
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
            Reporter reporter;
            get_last_restored_state().write_to_game(reporter, m_config, true);
            // we don't report here to reduce spam
            // restore every 2 frames so the game has a chance to update
            nn::os::YieldThread();
            nn::os::SleepThread(nn::TimeSpan::FromNanoSeconds(60 * 1000 * 1000));
        }
        break;
    case Command::RestoreDone:
        if (m_last_restored_ok) {
            if (m_config.m_show_restore_message) {
                if (m_last_restored_is_from_memory) {
                    msg::show_info("Restored state from memory");
                } else {
                    msg::show_info("Restored state from file");
                }
            }
            m_last_restored_ok = false;
        }
        break;
    case Command::SwitchMode:
        m_showed_welcome = true;
        if (m_controller.get_mode() == Controller::Mode::Active) {
            show_active_mode_message();
        }
        break;
    case Command::SaveOption:
        save_options();
        break;
    case Command::None:
        break;
    }
}

void Worker::save_options() const {
    DataWriter writer(OPTION_FILE_PATH);

    writer.write_integer("version", Version::vLatest);
    m_config.save_config(writer);
    m_controller.save_key_bindings(writer);
}

void Worker::load_options() {
    DataReader reader(OPTION_FILE_PATH);

    u32 version = 0;
    reader.read_integer(&version);
    if (version <= vLegacy || version > Version::vLatest) {
        return;
    }

    StateConfig temp_config;
    temp_config.read_config(reader);
    if (reader.is_successful()) {
        m_config = temp_config;
    }
    m_controller.load_key_bindings(reader);
}

static void report_fail_read(Reporter& reporter) {
    StringBuffer<280> report;
    report.append("Error! Failed to read game memory\n");
    reporter.append_fields_to(report);
    msg::show_widget(report.content());
}

static void report_fail_write(Reporter& reporter) {
    StringBuffer<280> report;
    report.append("Error! Failed to write game memory\n");
    reporter.append_fields_to(report);
    msg::show_widget(report.content());
}

void Worker::execute_save() {
    Reporter reporter;
    m_memory_state.read_from_game(reporter, m_config);
    if (reporter.has_error()) {
        report_fail_read(reporter);
    }

    msg::show_info("Saved state to memory");
}

void Worker::execute_save_file() {
    State temp_state;
    Reporter reporter;
    temp_state.read_from_game(reporter, m_config);
    if (reporter.has_error()) {
        report_fail_read(reporter);
        return;
    }

    DataWriter writer(STATE_FILE_PATH);
    StateFileResult result = temp_state.write_to_file(writer);
    if (result != StateFileResult::Ok || !writer.is_successful()) {
        msg::show_widget("Error!\nIO Error while writing state file");
        return;
    }
    msg::show_info("Saved state to file");
}

void Worker::execute_restore() {
    m_last_restored_ok = false;

    Reporter reporter;
    m_memory_state.write_to_game(reporter, m_config, false);
    if (reporter.has_error()) {
        report_fail_write(reporter);
        return;
    }
    m_last_restored_ok = true;
    m_last_restored_is_from_memory = true;
}

void Worker::execute_restore_file() {
    m_last_restored_ok = false;

    File file(STATE_FILE_PATH);
    if (!file.exists()) {
        msg::show_info("No restore.txt found!");
        return;
    }

    DataReader reader(STATE_FILE_PATH);
    StateFileResult result = m_last_restored_file.read_from_file(reader);
    switch (result) {
    case StateFileResult::UnsupportedVersion:
        msg::show_widget("The state file restore.txt\nhas an unsupported version.");
        return;
    case StateFileResult::InvalidVersion:
        msg::show_widget(
            "The state file restore.txt\nhas an invalid version. It could\nbe corrupted.");
        return;
    case StateFileResult::IOError:
        msg::show_widget("IO Error reading restore.txt!");
        return;
    case StateFileResult::Ok:
        break;
    }
    if (!reader.is_successful()) {
        msg::show_widget("IO Error reading restore.txt!");
        return;
    }

    Reporter reporter;
    m_last_restored_file.write_to_game(reporter, m_config, false);
    if (reporter.has_error()) {
        report_fail_write(reporter);
        return;
    }

    m_last_restored_ok = true;
    m_last_restored_is_from_memory = false;
}

bool Worker::show_active_mode_message() const {
    return msg::show_widgetf("\
Settings exited.\n\
You can use the save state now\n\
Hold All Triggers + Dpad Down\n\
to open Settings again");
}

void Worker::welcome() {
    if (m_showed_welcome) {
        return;
    }
    // probe to see if it's probably safe to deref havok position
    if (!ksys_ui_ScreenMgr_sInstance) {
        m_pos_diff_ticks = 0;
        return;
    }
    auto* pmdm = uking::ui::PauseMenuDataMgr::instance();
    if (!pmdm) {
        m_pos_diff_ticks = 0;
        return;
    }
    auto* proc_mgr = ksys::act::BaseProcMgr::instance();
    if (!proc_mgr) {
        m_pos_diff_ticks = 0;
        return;
    }

    float new_pos[3];
    if (!raw_ptr::havok_position().get_array(new_pos, 3)) {
        m_pos_diff_ticks = 0;
        return;
    }
    if (m_pos_diff_ticks > 0) {
        // y is not checked because of elevator cutscene
        if (std::abs(new_pos[0] - m_player_pos[0]) < 0.0001F ||
            std::abs(new_pos[2] - m_player_pos[2]) < 0.0001F) {
            m_pos_diff_ticks = 0;
            return;
        }
    }
    m_pos_diff_ticks++;
    if (m_pos_diff_ticks >= 4) {
        bool showed = msg::show_widget("Save State is active\n\nHold All Triggers + Dpad Down\nto "
                                       "open Settings, or see README\non GitHub for more info.");
        if (showed) {
            m_showed_welcome = true;
            return;
        }
    }
    m_player_pos[0] = new_pos[0];
    m_player_pos[1] = new_pos[1];
    m_player_pos[2] = new_pos[2];
}

}  // namespace botw::savs

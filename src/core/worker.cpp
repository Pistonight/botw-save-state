#include <Game/UI/uiPauseMenuDataMgr.h>
#include <KingSystem/ActorSystem/actActorSystem.h>
#include <KingSystem/ActorSystem/actBaseProcMgr.h>
#include <cstdlib>
#include <nn/os.h>
#include <nn/time.h>
#include <toolkit/io/data_reader.hpp>
#include <toolkit/io/data_writer.hpp>
#include <toolkit/mem/unique_ptr.hpp>
#include <toolkit/msg/info.hpp>
#include <toolkit/msg/widget.hpp>
#include <toolkit/tcp.hpp>

#include "core/reporter.hpp"
#include "core/time.hpp"
#include "core/version.hpp"
#include "core/worker.hpp"
#include "impl/raw_ptr.hpp"

extern "C" {
void* memalign(size_t alignment, size_t size);
}

namespace botw::savs {

static nn::os::ThreadType s_thread;

void worker_main(void*) {
    auto worker = mem::make_unique<Worker>();
    while (true) {
        // we need to trap the thread here if alloc fails
        if (worker && worker->initialize()) {
            break;
        }
        nn::os::YieldThread();
        nn::os::SleepThread(nn::TimeSpan::FromSeconds(5));
    }
    while (true) {
        worker->do_work();
        nn::os::YieldThread();
        nn::os::SleepThread(nn::TimeSpan::FromNanoSeconds(100000000));
    }
}

void start_worker_thread() {
    const u64 STACK_SIZE = 0x80000;
    void* thread_stack = memalign(0x1000, STACK_SIZE);

    nn::Result result = nn::os::CreateThread(&s_thread, worker_main, nullptr,
                                             thread_stack, STACK_SIZE, 16);
    if (result.IsFailure()) {
        return;
    }
    nn::os::StartThread(&s_thread);
}

void Worker::do_work() {
    welcome();
    tick_update();
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
            nn::os::SleepThread(
                nn::TimeSpan::FromNanoSeconds(60 * 1000 * 1000));
        }
        break;
    case Command::RestoreDone:
        if (m_last_restored_ok) {
            if (m_config.m_show_restore_message) {
                if (m_last_restored_is_from_memory) {
                    msg::info::print("Restored state from memory");
                } else {
                    msg::info::print("Restored state from file");
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
    io::DataWriter writer(OPTION_FILE_PATH);

    writer.write_integer("version", Version::vLatest);
    m_config.save_config(writer);
    m_controller.save_key_bindings(writer);
    writer.flush();
}

void Worker::load_options() {
    io::DataReader reader(OPTION_FILE_PATH);

    u32 version = 0;
    reader.read_integer(&version);
    if (version <= vLegacy || version > Version::vLatest) {
        return;
    }

    StateConfig temp_config;
    temp_config.read_config(reader, version);
    if (reader.is_successful()) {
        m_config = temp_config;
    }
    m_controller.load_key_bindings(reader);
}

static void report_fail_read(Reporter& reporter) {
    mem::StringBuffer<280> report;
    report.append("Error! Failed to read game memory\n");
    reporter.append_fields_to(report);
    msg::widget::print(report.content());
}

static void report_fail_write(Reporter& reporter) {
    mem::StringBuffer<280> report;
    report.append("Error! Failed to write game memory\n");
    reporter.append_fields_to(report);
    msg::widget::print(report.content());
}

void Worker::execute_save() {
    tcp::sendf("save to memory\n");
    Reporter reporter;
    m_memory_state.read_from_game(reporter, m_config);
    if (reporter.has_error()) {
        report_fail_read(reporter);
    }

    msg::info::print("Saved state to memory");
}

void Worker::execute_save_file() {
    tcp::sendf("save to file\n");
    mem::unique_ptr<State> temp_state = mem::make_unique<State>();
    if (!temp_state) {
        msg::widget::print("Error!\nFailed to allocate memory for state");
        return;
    }

    Reporter reporter;
    temp_state->read_from_game(reporter, m_config);
    if (reporter.has_error()) {
        report_fail_read(reporter);
        return;
    }

    tcp::sendf("writing latest.txt\n");
    io::DataWriter writer(STATE_FILE_PATH);
    StateFileResult result = temp_state->write_to_file(writer);
    writer.flush();
    if (result != StateFileResult::Ok || !writer.is_successful()) {
        msg::widget::print("Error!\nIO Error while writing state file");
        return;
    }
    msg::info::print("Saved state to file");
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

    io::File file(RESTORE_FILE_PATH);
    if (!file.exists()) {
        msg::info::print("No restore.txt found!");
        return;
    }

    io::DataReader reader(RESTORE_FILE_PATH);
    StateFileResult result = m_last_restored_file.read_from_file(reader);
    switch (result) {
    case StateFileResult::UnsupportedVersion:
        msg::widget::print(
            "The state file restore.txt\nhas an unsupported version.");
        return;
    case StateFileResult::InvalidVersion:
        msg::widget::print("The state file restore.txt\nhas an invalid "
                           "version. It could\nbe corrupted.");
        return;
    case StateFileResult::IOError:
        msg::widget::print("IO Error reading restore.txt!");
        return;
    case StateFileResult::Ok:
        break;
    }
    if (!reader.is_successful()) {
        msg::widget::print("IO Error reading restore.txt!");
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
    // clang-format off
    //                        ----------------------------------MAX
    return msg::widget::print("Settings exited\n\n"
                              "You can use the save state now\n"
                              "Hold All Triggers + Dpad Down\n"
                              "to open Settings again");
    // clang-format on
}

void Worker::tick_update() {
    if (m_controller.is_in_settings()) {
        return;
    }
    if (!m_showed_welcome) {
        // only update after showing welcome message
        // since we are reusing variables
        return;
    }
    if (m_config.m_speedometer) {
        // for some reason, ActorSystem::getPlayerPos() doesn't work
        float new_pos[3];
        if (!raw_ptr::havok_position().get_array(new_pos, 3)) {
            tcp::sendf("failed to get player pos\n");
            return;
        }
        // we can't use nn::time because the resolution is 1s
        f32 secs = time::ticks_to_secs_f32(1);
        f32 dx = new_pos[0] - m_player_pos[0];
        f32 dy = new_pos[1] - m_player_pos[1];
        f32 dz = new_pos[2] - m_player_pos[2];
        m_player_pos[0] = new_pos[0];
        m_player_pos[1] = new_pos[1];
        m_player_pos[2] = new_pos[2];
        bool pos_diff =
            std::abs(dx) > 0.1 || std::abs(dz) > 0.1 || std::abs(dy) > 0.1;
        if (!pos_diff) {
            tcp::sendf("pos diff too small\n");
            return;
        }
        tcp::sendf("pos: %.2f %.2f %.2f, d: %.2f %.2f %.2f\n", new_pos[0],
                   new_pos[1], new_pos[2], dx, dy, dz);
        f32 xsq_zsq = dx * dx + dz * dz;
        f32 horizontal_speed = std::sqrt(xsq_zsq) / secs;
        f32 true_speed = std::sqrt(dy * dy + xsq_zsq) / secs;
        msg::info::printf("HS: %.2f m/s | TS: %.2f m/s", horizontal_speed,
                          true_speed);
    }
}

void Worker::welcome() {
    if (m_showed_welcome) {
        return;
    }
    // probe to see if it's probably safe to deref havok position
    if (!msg::widget::is_ready()) {
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
        bool showed = msg::widget::print(
            "Save State is active\n\nHold All Triggers + Dpad Down\nto "
            "open Settings, or see README\non GitHub for more info.");
        if (showed) {
            m_showed_welcome = true;
            tcp::start_server(5001);
            return;
        }
    }
    m_player_pos[0] = new_pos[0];
    m_player_pos[1] = new_pos[1];
    m_player_pos[2] = new_pos[2];
}

} // namespace botw::savs

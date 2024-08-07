#include <cstring>

#include <exl/types.h>

#include "./message.hpp"

namespace botwsavs::util::msg {

static char16_t s_buffer[MESSAGE_BUFFER_LEN];
static u64 s_buffer_len = 0;
static u64 s_overlay_magic_index = 0;

u64 get_message_string_hook(void* file, sead::SafeString& msg_id, WideString* out) {
    if ((s_overlay_magic_index == 0x21 && msg_id == "0028") || (s_overlay_magic_index == 0x2A && msg_id == "0061")) {
        out->content = s_buffer;
        out->length = s_buffer_len;
        return 0;
    }

    return botw_get_message_string(file, &msg_id, out);
}

void show_custom(const char* message) {
    u64 i = 0;
    for (; i < MESSAGE_BUFFER_LEN; i++) {
        if (message[i] == '\0') {
            s_buffer[i] = '\0';
            break;
        }
        s_buffer[i] = static_cast<char16_t>(message[i]);
    }
    s_buffer_len = i;
    s_buffer[MESSAGE_BUFFER_LEN - 1] = '\0';
    if (s_overlay_magic_index == 0x21) {
        s_overlay_magic_index = 0x2A;
    } else {
        s_overlay_magic_index = 0x21;
    }

    show_info_by_idx(s_overlay_magic_index);
}

void show_info_by_idx(u64 idx) {
    const sead::SafeString useless("?");
    botw_show_info_overlay(idx, &useless);
}

void show_set_level(u32 level) {
    switch (level) {
    case 0:
        show_custom("Save state is now disabled");
        break;
    case 1:
        show_custom("Save state set to: Basic (Level 1)");
        break;
    case 2:
        show_custom("Save state set to: Basic + Durability (Level 2)");
        break;
    case 3:
        show_custom("Save state set to: All (Level 3)");
        break;
    }
}

void show_error(const char* error) {
    show_customf("ERROR: %s", error);
}

}  // namespace botwsavs::ui

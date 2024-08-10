#include <cstring>

#include <nn/os.h>
#include <exl/types.h>
#include <exl/lib.hpp>

#include "./message.hpp"

namespace botwsavs::util::msg {

static char16_t s_info_buffer[MESSAGE_BUFFER_LEN];
static u64 s_info_buffer_len = 0;
static char16_t s_widget_buffer[MESSAGE_BUFFER_LEN];
static u64 s_widget_buffer_len = 0;
static u32 s_info_overlay_idx = 0x2A;

// Hooking the message loader function
HOOK_DEFINE_TRAMPOLINE(KsysUiGetMessageHook) {
    static bool Callback(sead::SafeString* file, sead::SafeString* msg_id, WideString* out) {
        if (*file == "LayoutMsg/MainScreen_00") {
            // handle info overlay
            // output the previously set message
            if (*msg_id == "0061" || *msg_id == "0028") {
                out->content = s_info_buffer;
                out->length = s_info_buffer_len;
                return 0;
            }

        } else if (*file == "LayoutMsg/MessageTipsRunTime_00") {
            // handle widgets
            if (*msg_id == WIDGET_MSG_ID) {
                out->content = s_widget_buffer;
                out->length = s_widget_buffer_len;
                return 0;
            }
        }
        // original function for other cases
        return Orig(file, msg_id, out);
    }
};

HOOK_DEFINE_TRAMPOLINE(KsysUiInitMessageTipsRuntimeHook) {
    static void Callback() {
        Orig();
        ksys_ui_runtime_tips[0x0E].m_label = "0025";
        ksys_ui_runtime_tips[0x0E].m_flag = "";
    }
};

HOOK_DEFINE_TRAMPOLINE(ScreenMessageTipsRuntimeShowMessageTipHook) {
    static void Callback(void* this_, u32 idx, bool) {
        if (idx == 0x17 || idx == 0x0E) {
            u32 set_idx = idx == 0x17 ? 0x0E : 0x17;
            (reinterpret_cast<bool*>(this_))[0x365C] = false;
            (reinterpret_cast<int*>(this_))[0xD96] = set_idx;
            (reinterpret_cast<int*>(this_))[0xD98] = set_idx;
            nn::os::YieldThread();
            nn::os::SleepThread(nn::TimeSpan::FromSeconds(1));
        }
        return Orig(this_, idx, true);
    }
};

namespace inst = exl::armv8::inst;

void install_hooks() {
    KsysUiGetMessageHook::InstallAtOffset(0x123DEA0);
    KsysUiInitMessageTipsRuntimeHook::InstallAtOffset(0x20950);
    ScreenMessageTipsRuntimeShowMessageTipHook::InstallAtOffset(0x119C750);

    // patch ScreenMessageTipsRuntime::doShowMessageTip(eui::Screen* this, idx, bool)
    // 0x119C750 in 1.6.0
    /* exl::patch::CodePatcher patcher { 0x0119C788 }; */
    // patching out checking if previous message is the same
    /* patcher.WriteInst(inst::Nop()); */
    /* patcher.WriteInst(inst::Nop()); */
    /* patcher.WriteInst(inst::Nop()); */
    /* patcher.WriteInst(inst::Nop()); */
    /* patcher.WriteInst(inst::Nop()); */
}

/* static u64 s_overlay_magic_index = 0; */

/* u64 get_message_string_hook(void* file, sead::SafeString& msg_id, WideString* out) { */
/*     if ((s_overlay_magic_index == 0x21 && msg_id == "0028") || (s_overlay_magic_index == 0x2A && msg_id == "0061")) { */
/*         out->content = s_buffer; */
/*         out->length = s_buffer_len; */
/*         return 0; */
/*     } */
/*  */
/*     return botw_get_message_string(file, &msg_id, out); */
/* } */

void show_custom(const char* message) {
    u64 i = 0;
    for (; i < MESSAGE_BUFFER_LEN; i++) {
        if (message[i] == '\0') {
            s_info_buffer[i] = '\0';
            break;
        }
        s_info_buffer[i] = static_cast<char16_t>(message[i]);
    }
    s_info_buffer_len = i;
    s_info_buffer[MESSAGE_BUFFER_LEN - 1] = '\0';
    if (s_info_overlay_idx == 0x21) {
        s_info_overlay_idx = 0x2A;
    } else {
        s_info_overlay_idx = 0x21;
    }

    ksys_ui_showInfoOverlayWithString(s_info_overlay_idx, &sead::SafeStringBase<char>::cEmptyString);
}

static int s_test_idx = 0x17;

void show_custom_widget(const char* message) {
    if (!ksys_ui_screen_mgr_instance) {
        return;
    }
    void** screens = ksys_ui_screen_mgr_instance->screens;
    if (ksys_ui_screen_mgr_instance->num_screens > 0x29) {
        screens += 0x29;
    }
    void* screen = *screens;
    // should be a sead::DynamicCast here
    if (!screen) {
        return;
    }
    if (s_test_idx == 0x17) {
        s_test_idx = 0x0E;
    } else {
        s_test_idx = 0x17;
    }
    u64 i = 0;
    for (; i < MESSAGE_BUFFER_LEN; i++) {
        if (message[i] == '\0') {
            s_widget_buffer[i] = '\0';
            break;
        }
        s_widget_buffer[i] = static_cast<char16_t>(message[i]);
    }
    s_widget_buffer_len = i;
    s_widget_buffer[MESSAGE_BUFFER_LEN - 1] = '\0';
    ScreenMessageTipsRuntime_doShowMessageTip(screen, s_test_idx, true);
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

}  // namespace botwsavs::ui

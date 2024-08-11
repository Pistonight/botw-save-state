#include "util/message.hpp"
#include <cstring>
#include <exl/lib.hpp>
#include <exl/types.h>
#include <nn/os.h>
#include <stdarg.h>
#include <stdio.h>
#include "util/string.hpp"

namespace botw::savs::msg {

// can display around 40 chars without truncation in US and EU region languages
constexpr size_t INFO_BUFFER_LEN = 40;
constexpr size_t WIDGET_BUFFER_LEN = 280;
constexpr const char* WIDGET_MSG_ID = "0025";

static WStringBuffer<INFO_BUFFER_LEN> s_info;
static u32 s_info_idx = 0;
static WStringBuffer<WIDGET_BUFFER_LEN> s_widget;
static u32 s_widget_idx = 0;

// Hooking the message loader function
HOOK_DEFINE_TRAMPOLINE(KsysUiGetMessageHook){
    static bool Callback(sead::SafeString * file, sead::SafeString* msg_id,
                         WideString* out){if (*file == "LayoutMsg/MainScreen_00"){
        // handle info overlay
        // 0028 - The Master Sword has returned to the forest
        // 0061 - In this demo version, you can't advance any farther
        if (*msg_id == "0061" || *msg_id == "0028"){// output the previously set message
                                                    out->content = s_info.content();
;
out->length = s_info.len();
return 0;
}  // namespace botw::savs::msg
}
else if (*file == "LayoutMsg/MessageTipsRunTime_00") {
    // handle widgets
    if (*msg_id == WIDGET_MSG_ID) {
        out->content = s_widget.content();
        out->length = s_widget.len();
        return 0;
    }
}
// original function for other cases
return Orig(file, msg_id, out);
}
}
;

// Hooking the initialization of message tip
// to override the "Wolf Link" text and flag
HOOK_DEFINE_TRAMPOLINE(KsysUiInitMessageTipsRuntimeHook){static void Callback(){Orig();
ksys_ui_sRuntimeTips[0x0E].m_label = WIDGET_MSG_ID;
// note that this will not work for 1.5.0
// 1.6.0 added a message for VR mode without a flag,
// so they added a check for skipping the flag check when
// the flag is empty
ksys_ui_sRuntimeTips[0x0E].m_flag = "";
}
}
;

HOOK_DEFINE_TRAMPOLINE(ScreenMessageTipsRuntimeShowMessageTipHook){
    static void Callback(void* this_, u32 idx, bool){
        if (idx == 0x17 || idx == 0x0E){u32 set_idx = idx == 0x17 ? 0x0E : 0x17;
(reinterpret_cast<bool*>(this_))[0x365C] = false;
(reinterpret_cast<int*>(this_))[0xD96] = set_idx;
(reinterpret_cast<int*>(this_))[0xD98] = set_idx;
nn::os::YieldThread();
nn::os::SleepThread(nn::TimeSpan::FromNanoSeconds(100 * 1000 * 1000));
}
return Orig(this_, idx, true);
}
}
;

/* namespace inst = exl::armv8::inst; */

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

void show_info(const char* message) {
    s_info.copy_from(message);
    // cycle the message index so the game would clear the previous one
    s_info_idx = s_info_idx == 0x21 ? 0x2A : 0x21;
    ksys_ui_showInfoOverlayWithString(s_info_idx, &sead::SafeStringBase<char>::cEmptyString);
}

void show_infof(const char* format, ...) {
    char result[INFO_BUFFER_LEN + 1];
    result[INFO_BUFFER_LEN - 1] = '\0';
    va_list args;
    va_start(args, format);
    int size = vsnprintf(result, INFO_BUFFER_LEN, format, args);
    va_end(args);
    if (size <= 0) {
        return;
    }
    result[size] = '\0';
    show_info(result);
}

bool show_widget(const char* message) {
    if (!ksys_ui_ScreenMgr_sInstance) {
        return false;
    }
    // idk what this is
    void** screens = ksys_ui_ScreenMgr_sInstance->screens;
    if (ksys_ui_ScreenMgr_sInstance->num_screens > 0x29) {
        screens += 0x29;
    }
    void* screen = *screens;
    // should be a sead::DynamicCast here
    if (!screen) {
        return false;
    }
    s_widget.copy_from(message);
    // cycle the message index so the game would clear the previous one
    /* (reinterpret_cast<bool*>(screen))[0x365C] = false; */
    /* (reinterpret_cast<int*>(screen))[0xD96] = s_widget_idx; */
    /* (reinterpret_cast<int*>(screen))[0xD98] = s_widget_idx; */
    // ^ totally safe don't look
    // let screen mgr delete the previous message
    /* nn::os::YieldThread(); */
    /* nn::os::SleepThread(nn::TimeSpan::FromNanoSeconds(1000 * 1000 * 1000)); */
    s_widget_idx = s_widget_idx == 0x17 ? 0x0E : 0x17;
    ScreenMessageTipsRuntime_doShowMessageTip(screen, s_widget_idx, true);
    // yield thread to let the screen manager pick up the new message quicker
    /* nn::os::YieldThread(); */
    return true;
}

bool show_widgetf(const char* format, ...) {
    char result[WIDGET_BUFFER_LEN + 1];
    result[WIDGET_BUFFER_LEN - 1] = '\0';
    va_list args;
    va_start(args, format);
    int size = vsnprintf(result, WIDGET_BUFFER_LEN, format, args);
    va_end(args);
    if (size <= 0) {
        return false;
    }
    result[size] = '\0';
    return show_widget(result);
}

}  // namespace botwsavs::ui

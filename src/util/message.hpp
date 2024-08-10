/**
 * This is a patch for BOTW 1.6.0 to show custom messages on the screen
 * This overrides:
 * - The info overlay message (such as "Your Pot Lid is badly damaged")
 * - The tip widget message (such as "The area is very cold ...")
 *
 * The info overlay message overrides an unused demo version message,
 * with idx=0x2A and msg_id="0061". When this string is requested,
 * the overriden message is provided instead.
 *
 */
#pragma once
#include <cstddef>
#include <exl/types.h>
#include <prim/seadSafeString.h>
#include <prim/seadRuntimeTypeInfo.h>
#include <stdio.h>

struct WideString {// This is probably a eui::MessageString
    char16_t* content = nullptr;
    u64 length = 0;
};

struct RuntimeTip {
    sead::SafeString m_label;
    sead::SafeString m_flag;
};
static_assert(sizeof(RuntimeTip) == 0x20);

struct ScreenMgr {
    void* vtable;
    char disposer[0x20];
    u32 num_screens;
    void** screens;
};
static_assert(offsetof(ScreenMgr, num_screens) == 0x28);
static_assert(offsetof(ScreenMgr, screens) == 0x30);

extern "C" {
// BOTW Symbols
// These can be replaced with the decomp headers once they are available

// 0x0123DEA0 (1.6.0)
// 0x00AA248C (1.5.0)
// Load msg_id from file and put it in out
// file is 
//   - "LayoutMsg/MessageTipsRunTime_00" for widgets
//   - "LayoutMsg/MainScreen_00" for info overlay
//
// msg_id is like "0001"
// return 0 for success
u64 ksys_ui_getMessage(
    sead::SafeString* file, sead::SafeString* msg_id, WideString* out);

// 0x01238680 (1.6.0)
// 0x00A95924 (1.5.0)
void* ksys_ui_showInfoOverlayWithString(
    u64 idx, const sead::SafeString* info);

// 0x0119C750 (1.6.0)
void ScreenMessageTipsRuntime_doShowMessageTip(void* this_, u32 idx, bool);

// 0x00020950 (1.6.0)
// 0x00A261CC (1.5.0)
void ksys_ui_initMessageTipsRuntime();

// 0x02CBA3A8 (1.6.0)
// 0x025EFC08 (1.5.0)
/* extern u32 ksys_ui_runtime_tip_num; */

// 0x02CBA3B0 (1.6.0)
// 0x025EFC10 (1.5.0)
extern RuntimeTip* ksys_ui_runtime_tips;

// 0x02CC2490 (1.6.0)
// 0x025FCC68 (1.5.0)
extern ScreenMgr* ksys_ui_screen_mgr_instance;
}


namespace botwsavs::util::msg {

constexpr const char* INFO_OVERLAY_MSG_ID = "0061";
constexpr const char* WIDGET_MSG_ID = "0025";

/**
 * Initialize the hook system to override the messages
 */
void install_hooks();
void show_custom_widget(const char* message);

constexpr size_t MESSAGE_BUFFER_LEN = 200;

/**
 * Show a custom message on the screen
 */
void show_custom(const char* message);

/**
 * Show a custom formatted message on the screen
 */
template <typename T>
void show_customf(const char* format, T value) {
    char result[MESSAGE_BUFFER_LEN];
    snprintf(result, MESSAGE_BUFFER_LEN, format, value);
    result[MESSAGE_BUFFER_LEN - 1] = '\0';
    show_custom(result);
}


/**
 * Show message when switching save state level
 */
void show_set_level(u32 level);

}  // namespace botwsavs::util

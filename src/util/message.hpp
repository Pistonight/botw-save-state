/**
 * This is a patch for BOTW 1.6.0 to show custom messages on the screen
 * This overrides:
 * - The info overlay message (such as "Your Pot Lid is badly damaged")
 * - The tip widget message (such as "The area is very cold ...")
 *
 * The basic idea is to hook into the function that loads the actual message
 * to load the overridden message instead. Then call the functions to display
 * the message.
 *
 */
#pragma once
#include <cstddef>
#include <exl/types.h>
#include <prim/seadRuntimeTypeInfo.h>
#include <prim/seadSafeString.h>

struct WideString {  // This is probably a eui::MessageString
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
/* u64 ksys_ui_getMessage( */
/*     sead::SafeString* file, sead::SafeString* msg_id, WideString* out); */

// 0x01238680 (1.6.0)
// 0x00A95924 (1.5.0)
void* ksys_ui_showInfoOverlayWithString(u64 idx, const sead::SafeString* info);

// 0x0119C750 (1.6.0)
void ScreenMessageTipsRuntime_doShowMessageTip(void* this_, u32 idx, bool);

// 0x00020950 (1.6.0)
// 0x00A261CC (1.5.0)
/* void ksys_ui_initMessageTipsRuntime(); */

// 0x02CBA3A8 (1.6.0)
// 0x025EFC08 (1.5.0)
/* extern u32 ksys_ui_sRuntimeTipsNum; */

// 0x02CBA3B0 (1.6.0)
// 0x025EFC10 (1.5.0)
extern RuntimeTip* ksys_ui_sRuntimeTips;

// 0x02CC2490 (1.6.0)
// 0x025FCC68 (1.5.0)
extern ScreenMgr* ksys_ui_ScreenMgr_sInstance;
}

namespace botw::savs::msg {

/**
 * Initialize the hook system to override the messages
 */
void install_hooks();

/**
 * Show a custom info message on the screen
 */
void show_info(const char* message);

/**
 * Show a custom formatted info message on the screen
 */
void show_infof(const char* format, ...);

/**
 * Show a custom widget message on the top-right corner of the screen
 */
bool show_widget(const char* message);

/**
 * Show a custom formatted widget message on the top-right corner of the screen
 */
bool show_widgetf(const char* message, ...);

}  // namespace botw::savs::msg

#pragma once
#include <exl/types.h>
#include <prim/seadSafeString.h>
#include <stdio.h>

extern "C" u64 botw_get_message_string(void* file, void* msg_id, void* out);
extern "C" void* botw_show_info_overlay(u64 idx, const sead::SafeString* useless);

namespace botwsavs::util::msg {

constexpr size_t MESSAGE_BUFFER_LEN = 200;

struct WideString {
    char16_t* content = nullptr;
    u64 length = 0;
};
/** 
 * Hook to make the game get the custom message instead of the original message
 */
u64 get_message_string_hook(void* file, sead::SafeString& msg_id, WideString* out);

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
 * Show an original message by its id
 */
void show_info_by_idx(u64 idx);

/**
 * Show "Can't do that right now"
 */
inline void show_cant_do_that_right_now() {
    show_info_by_idx(0x22);
}

/**
 * Show message when switching save state level
 */
void show_set_level(u32 level);

}  // namespace botwsavs::util

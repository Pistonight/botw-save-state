#pragma once
#include <prim/seadSafeString.h>
#include <stdio.h>
#include "types.h"

namespace botwsavs::ui {

struct WideString {
    wchar* content = nullptr;
    u64 length = 0;
};

u64 GetMessageStringHook(void* file, sead::SafeString& messageId, WideString* outString);

void ShowOverridenMessage(const char* message);

template <typename T>
void ShowFormattedMessage(const char* format, T& value) {
    char result[200];
    snprintf(result, 200, format, value);
    ShowOverridenMessage(result);
}

void ShowOriginalMessage(u64 idx);

inline void ShowCantDoThatRightNow() {
    ShowOriginalMessage(0x22);
}

inline void ShowSetLevel(u32 level) {
    switch (level) {
    case 0:
        ShowOverridenMessage("Save state is now disabled");
        break;
    case 1:
        ShowOverridenMessage("Save state set to: Basic");
        break;
    case 2:
        ShowOverridenMessage("Save state set to: Basic + Durability");
        break;
    case 3:
        ShowOverridenMessage("Save state set to: All");
        break;
    }
}

inline void ShowError() {
    ShowOverridenMessage("Save state error!");
}

inline void ShowLevelError(u32 stateLevel) {
    ShowFormattedMessage("You need to lower the setting level to %d to restore!", stateLevel);
}

}  // namespace botwsavs::ui

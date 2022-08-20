#pragma once
#include <prim/seadSafeString.h>
#include "types.h"

namespace botwsavs::ui {

struct WideString {
    wchar* content = nullptr;
    u64 length = 0;
};

u64 GetMessageStringHook(void* file, sead::SafeString& messageId, WideString* outString);

void ShowOverridenMessage(const char* message);

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

inline void ShowLevelError() {
    ShowOverridenMessage("You need to lower the setting level to restore!");
}

}  // namespace botwsavs::ui

#include "OverlayString.hpp"
#include <cstring>

#include "mem/KingPtr.hpp"
#include "util/StringBuffer.hpp"

#include "types.h"

#define BUF_LEN 200

namespace botwsavs::ui {

static wchar sDisplayBuffer[BUF_LEN];
static u64 sDisplayBufferLength = 0;
static u64 sIdx = 0;

u64 GetMessageStringHook(void* file, sead::SafeString& messageId, WideString* outString) {
    if ((sIdx == 0x21 && messageId == "0028") || (sIdx == 0x2A && messageId == "0061")) {
        outString->content = sDisplayBuffer;
        outString->length = sDisplayBufferLength;
        return 0;
    }

    return mem::KingPtr::GetMessageString(file, &messageId, outString);
}

void ShowOverridenMessage(const char* message) {
    u64 i = 0;
    for (; i < BUF_LEN; i++) {
        if (message[i] == '\0') {
            sDisplayBuffer[i] = '\0';
            break;
        }
        sDisplayBuffer[i] = static_cast<wchar>(message[i]);
    }
    sDisplayBufferLength = i;
    sDisplayBuffer[BUF_LEN - 1] = '\0';
    if (sIdx == 0x21) {
        sIdx = 0x2A;
    } else {
        sIdx = 0x21;
    }

    ShowOriginalMessage(sIdx);
}

void ShowOriginalMessage(u64 idx) {
    sead::SafeString sUselessString("?");
    mem::KingPtr::ShowInfoOverlayWithString(idx, reinterpret_cast<const void*>(&sUselessString));
}

}  // namespace botwsavs::ui

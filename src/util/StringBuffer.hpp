#pragma once

#include <cstring>
#include <stdio.h>

#include "types.h"

namespace botwsavs::util {

template <u32 L>
class StringBuffer {
public:
    StringBuffer() { Clear(); }
    void Clear() {
        mLen = 0;
        EnsureTermination();
    }
    const char* Content() const { return mContent; }
    char* Last() { return mContent + mLen; }
    u32 Len() const { return mLen; }
    u32 StrLen() const { return strlen(mContent); }

    void IncLen(u32 size) {
        if (size > 0) {
            mLen += size;
            if (mLen > L) {
                mLen = L;
            }
            EnsureTermination();
        }
    }

    void SafeAppend(const char* text) {
        strncpy(Last(), text, L - mLen);
        IncLen(strlen(text));
    }

    template <typename T>
    void SafeAppendF(const char* format, T value) {
        IncLen(sprintf(Last(), format, value));
    }

    void EnsureTermination() { mContent[mLen] = '\0'; }

    bool IndexOf(char search, u32 from, u32* outIndex) {
        for (u32 i = from; i < mLen; i++) {
            if (mContent[i] == search) {
                *outIndex = i;
                return true;
            }
        }
        return false;
    }

    void SafeSet(u32 i, char c) {
        if (i < mLen) {
            mContent[i] = c;
        }
    }

    void SafeDeleteFront(u32 size) {
        if (size > mLen) {
            Clear();
            return;
        }
        for (u32 i = 0; i < mLen - size; i++) {
            mContent[i] = mContent[i + size];
        }
        mLen -= size;
        EnsureTermination();
    }

    void SafeDeleteEnd(u32 size) {
        if (mLen <= size) {
            mLen = 0;
        } else {
            mLen -= size;
        }
        EnsureTermination();
    }

private:
    char mContent[L + 1];
    u32 mLen;
};

}  // namespace botwsavs::util

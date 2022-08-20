#pragma once

#include "fs/Logger.hpp"
#include "types.h"

namespace botwsavs::util {

template <typename T>
class SafePtr {
public:
    SafePtr(T* ptr) : mPtr(ptr) {}
    SafePtr(const SafePtr& other) : mPtr(other.mPtr) {}

    bool Get(T* out) const {
        if (!LooksSafe()) {
            return false;
        }
        *out = *mPtr;
        return true;
    }

    bool Set(T value) {
        if (!LooksSafe()) {
            return false;
        }
        *mPtr = value;
        return true;
    }

    bool Get(T* out, u32 i) const {
        if (!LooksSafe()) {
            return false;
        }
        *out = mPtr[i];
        return true;
    }

    bool Set(T value, u32 i) {
        if (!LooksSafe()) {
            return false;
        }
        mPtr[i] = value;
        return true;
    }

    bool GetArray(T* out, u32 len) const {
        if (!LooksSafe()) {
            return false;
        }
        for (u32 i = 0; i < len; i++) {
            out[i] = mPtr[i];
        }
        return true;
    }

    bool SetArray(const T* array, u32 len) {
        if (!LooksSafe()) {
            return false;
        }
        for (u32 i = 0; i < len; i++) {
            mPtr[i] = array[i];
        }
        return true;
    }

private:
    bool LooksSafe() const {
        u64 raw = reinterpret_cast<u64>(mPtr);

        if (raw > 0xFFFFFFFFFF || (raw >> 32 == 0)) {
            warnf("Pointer %p does not look safe", mPtr);
            return false;
        }
        if ((raw & 3) != 0) {
            warnf("Pointer %p does not look safe", mPtr);
            return false;
        }

        return true;
    }

private:
    T* mPtr;
};

}  // namespace botwsavs::util

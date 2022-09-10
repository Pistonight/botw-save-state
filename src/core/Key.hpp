#pragma once

#include "types.h"
#include "util/StringBuffer.hpp"

namespace botwsavs::core {
enum Key : u32 {
    A = 0,
    B = 1 << 1,
    ZL = 1 << 2,
    Y = 1 << 3,
    X = 1 << 4,
    ZR = 1 << 5,
    RStick = 1 << 6,
    LStick = 1 << 7,

    Minus = (1 << 9) | (1 << 12),
    Plus = (1 << 10) | (1 << 11),

    L = 1 << 13,
    R = 1 << 14,

    DpadUp = 1 << 16,
    DpadDown = 1 << 17,
    DpadLeft = 1 << 18,
    DpadRight = 1 << 19
};

namespace key {
template <u32 L>
void GetKeyString(u32 mask, util::StringBuffer<L>& outBuffer) {
    static_assert(L >= 100, "Key string buffer length should be at least 100");
    outBuffer.Clear();
    if (mask == 0) {
        return;
    }
    if (mask & Key::A) {
        outBuffer.SafeAppend("A+");
    }
    if (mask & Key::B) {
        outBuffer.SafeAppend("B+");
    }
    if (mask & Key::X) {
        outBuffer.SafeAppend("X+");
    }
    if (mask & Key::Y) {
        outBuffer.SafeAppend("Y+");
    }
    if (mask & Key::L) {
        outBuffer.SafeAppend("L+");
    }
    if (mask & Key::R) {
        outBuffer.SafeAppend("R+");
    }
    if (mask & Key::ZL) {
        outBuffer.SafeAppend("ZL+");
    }
    if (mask & Key::ZR) {
        outBuffer.SafeAppend("ZR+");
    }
    if (mask & Key::DpadUp) {
        outBuffer.SafeAppend("DpadUp+");
    }
    if (mask & Key::DpadDown) {
        outBuffer.SafeAppend("DpadDown+");
    }
    if (mask & Key::DpadLeft) {
        outBuffer.SafeAppend("DpadLeft+");
    }
    if (mask & Key::DpadRight) {
        outBuffer.SafeAppend("DpadRight+");
    }
    if (mask & Key::Minus) {
        outBuffer.SafeAppend("Minus+");
    }
    if (mask & Key::Plus) {
        outBuffer.SafeAppend("Plus+");
    }
    if (mask & Key::LStick) {
        outBuffer.SafeAppend("LStick+");
    }
    if (mask & Key::RStick) {
        outBuffer.SafeAppend("RStick+");
    }
    outBuffer.SafeDeleteEnd(1);
}
}  // namespace key
}  // namespace botwsavs::core

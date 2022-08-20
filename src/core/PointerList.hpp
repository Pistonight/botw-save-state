#pragma once
#include "MemPtr.hpp"

#define main(path, type) ((MemPtr() path).AsPtrTo<type>())

namespace botwsavs::core::ptr {
//[[main+2CA1A78]+80]+848
util::SafePtr<u32> Health() {
    return main([0x2CA1A78][0x80] + 0x848, u32);
}

//[[main+2c9fd70]+38]+2ac
util::SafePtr<f32> Stamina() {
    return main([0x2c9fd70][0x38] + 0x2ac, f32);
}

//[[[main+2D37AF0]+F0]+D28]+5E0 length=3
util::SafePtr<f32> HavokPosition() {
    return main([0x2D37AF0][0xF0][0xD28] + 0x5E0, f32);
}

//[[main+2CA1140]+60]+398 length=12
util::SafePtr<f32> MainPositionMatrix() {
    return main([0x2CA1140][0x60] + 0x398, f32);
}

//[[main+2CE1090]+60]+20 length=12
util::SafePtr<f32> CameraPanMatrix() {
    return main([0x2CE1090][0x60] + 0x20, f32);
}

//[[[main+2CA5520]+20]+558]+D48
util::SafePtr<f32> CameraZoom() {
    return main([0x2CA5520][0x20][0x558] + 0xD48, f32);
}

//[[[main+2CA5520]+20]+558]+D14
util::SafePtr<f32> CameraTilt() {
    return main([0x2CA5520][0x20][0x558] + 0xD14, f32);
}

//[[main+2CA3AD8]+30]+80 set to 360
util::SafePtr<f32> RoundBombCooldown() {
    return main([0x2CA3AD8][0x30] + 0x80, f32);
}

//[[main+2CA3AD8]+30]+98 set to 360
util::SafePtr<f32> SquareBombCooldown() {
    return main([0x2CA3AD8][0x30] + 0x98, f32);
}

//[[main+2CA3AD8]+30]+144 set to 1
util::SafePtr<f32> StasisCooldown() {
    return main([0x2CA3AD8][0x30] + 0x144, f32);
}

//[[main+2CA1140]+60]+20D0
util::SafePtr<f32> FallDamageY() {
    return main([0x2CA1140][0x60] + 0x20D0, f32);
}

}  // namespace botwsavs::core::ptr

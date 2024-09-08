#pragma once

#include <exl/types.h>

namespace botw::savs::time {

inline float ticks_to_secs_f32(u32 ticks) {
    // 1 tick = 3 frames
    // 1s = 30 frames = 10 ticks
    return ticks / 10.0F;
}

inline u32 secs_to_ticks(u32 secs) {
    // 1 tick = 3 frames
    // 1s = 30 frames = 10 ticks
    return secs * 10;
}

} // namespace botw::savs::time

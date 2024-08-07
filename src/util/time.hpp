#pragma once

#include "./string.hpp"

namespace botwsavs::util {
using TimeBuffer = StringBuffer<20>;
/**
 * Get current time as string, format YYYY-MM-DD HH:MM:SS
 */
void get_current_time(TimeBuffer& outBuffer);
}  // namespace botwsavs::util

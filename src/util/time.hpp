#pragma once

#include "util/string.hpp"

namespace botw::savs {
using TimeBuffer = StringBuffer<20>;
/**
 * Get current time as string, format YYYY-MM-DD HH:MM:SS
 */
void get_current_time(TimeBuffer& outBuffer);
}  // namespace botwsavs::util

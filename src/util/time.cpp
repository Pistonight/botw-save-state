#include <nn/time.h>

#include "./time.hpp"

namespace botwsavs::util {

void get_current_time(TimeBuffer& out) {
    out.clear();
    if (!nn::time::IsInitialized()) {
        out.append("0000-00-00 00:00:00");
        return;
    }
    nn::time::PosixTime posixTime;
    nn::time::CalendarAdditionalInfo calendarAddtionalInfo;
    nn::time::CalendarTime calendarTime;
    nn::time::StandardUserSystemClock::GetCurrentTime(&posixTime);
    nn::time::ToCalendarTime(&calendarTime, &calendarAddtionalInfo, posixTime);

    out.appendf("%04hu-", calendarTime.year);
    out.appendf("%02hu-", calendarTime.month);
    out.appendf("%02hu ", calendarTime.day);
    out.appendf("%02hu:", calendarTime.hour);
    out.appendf("%02hu:", calendarTime.minute);
    out.appendf("%02hu", calendarTime.second);
}

}  // namespace botwsavs::util

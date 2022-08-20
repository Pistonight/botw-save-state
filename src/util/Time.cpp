#include <nn/time.h>

#include "Time.hpp"

namespace botwsavs::util {

void CurrentTimeString(TimeBuffer& outBuffer) {
    if (!nn::time::IsInitialized()) {
        nn::Result result = nn::time::Initialize();
        if (result.IsFailure()) {
            return;
        }
    }
    nn::time::PosixTime posixTime;
    nn::time::CalendarAdditionalInfo calendarAddtionalInfo;
    nn::time::CalendarTime calendarTime;
    nn::time::StandardUserSystemClock::GetCurrentTime(&posixTime);
    nn::time::ToCalendarTime(&calendarTime, &calendarAddtionalInfo, posixTime);

    outBuffer.Clear();
    outBuffer.SafeAppendF("%04hu-", calendarTime.year);
    outBuffer.SafeAppendF("%02hu-", calendarTime.month);
    outBuffer.SafeAppendF("%02hu ", calendarTime.day);
    outBuffer.SafeAppendF("%02hu:", calendarTime.hour);
    outBuffer.SafeAppendF("%02hu:", calendarTime.minute);
    outBuffer.SafeAppendF("%02hu", calendarTime.second);
}

}  // namespace botwsavs::util

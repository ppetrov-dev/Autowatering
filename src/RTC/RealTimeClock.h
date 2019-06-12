#ifndef realTimeClockH
#define realTimeClockH

#include "Sodaq_DS3231.h"

class RealTimeClock
{
private:
    Sodaq_DS3231 _rtc;
    static char _weekDay[7][4];

public:
    String GetStringNow();
    void Begin();
    DateTime GetNow();
    long GetNowTimestamp();

    static String GetStringDateTime(DateTime dateTime);
};
#endif
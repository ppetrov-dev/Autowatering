#include "RealTimeClock.h"

char RealTimeClock::_weekDay[7][4] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

void RealTimeClock::Begin()
{
    _rtc.begin();
}

DateTime RealTimeClock::GetNow()
{
    return _rtc.now();
}

String RealTimeClock::GetStringNow()
{
    auto now = GetNow();
    return GetStringDateTime(now);
}

String RealTimeClock::GetStringDateTime(DateTime dateTime)
{
    String stringBuilder;
    stringBuilder.concat(_weekDay[dateTime.dayOfWeek()]);
    stringBuilder.concat(", " + String(dateTime.year()) + '/');
    stringBuilder.concat(String(dateTime.month()) + '/');
    stringBuilder.concat(String(dateTime.date()) + ' ');
    stringBuilder.concat(String(dateTime.hour()) + ':');
    stringBuilder.concat(String(dateTime.minute()) + ':');
    stringBuilder.concat(String(dateTime.second()));
    return stringBuilder;
}

long RealTimeClock::GetNowTimestamp()
{
    auto now = GetNow();
    return now.getEpoch();
}
#include "WaitTime.h"

WaitTime::WaitTime(long days, long hours, long minutes)
    : TimeBase(hours, minutes)
{
    _days = days;
}
void WaitTime::ConstrainDays()
{
    if (_days > 9)
        _days = 0;
    if (_days < 0)
        _days = 9;
}

unsigned long WaitTime::ToSeconds()
{
    return TimeBase::ToSeconds() + MyDateTimeConverters::DaysToSeconds(_days);
}

void WaitTime::UpdateValuesFromSeconds(unsigned long seconds)
{

    auto days = MyDateTimeConverters::SecondsToDays(seconds);
    auto secondsWithoutDays = seconds - MyDateTimeConverters::DaysToSeconds(days);
    _days = days;

    TimeBase::UpdateValuesFromSeconds(secondsWithoutDays);
}

void WaitTime::ChangeDays(int increment)
{
    _days += increment;
    ConstrainDays();
}

unsigned long WaitTime::GetDays()
{
    return _days;
}
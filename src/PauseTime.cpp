#include "PauseTime.h"

PauseTime::PauseTime(long days, long hours, long minutes)
    : TimeBase(hours, minutes)
{
    _days = days;
}
void PauseTime::ConstrainDays()
{
    if (_days > 9)
        _days = 0;
    if (_days < 0)
        _days = 9;
}

unsigned long PauseTime::ToSeconds()
{
    return TimeBase::ToSeconds() + Converters::DaysToSeconds(_days);
}

void PauseTime::UpdateValuesFromSeconds(unsigned long seconds)
{

    auto days = Converters::SecondsToDays(seconds);
    auto secondsWithoutDays = seconds - Converters::DaysToSeconds(days);
    _days = days;

    TimeBase::UpdateValuesFromSeconds(secondsWithoutDays);
}

void PauseTime::ChangeDays(int increment)
{
    _days += increment;
    ConstrainDays();
}

unsigned long PauseTime::GetDays()
{
    return _days;
}
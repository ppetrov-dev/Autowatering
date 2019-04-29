#include "WorkTime.h";
WorkTime::WorkTime(long hours, long minutes, long seconds)
    : TimeBase(hours, minutes)
{
    _seconds = seconds;
}
void WorkTime::ConstrainSeconds()
{
    if (_seconds > 59)
        _seconds = 0;
    if (_seconds < 0)
        _seconds = 59;
}

unsigned long WorkTime::ToSeconds()
{
    return TimeBase::ToSeconds() + _seconds;
}

void WorkTime::UpdateValuesFromSeconds(unsigned long seconds)
{

    TimeBase::UpdateValuesFromSeconds(seconds);
    _seconds = seconds - TimeBase::ToSeconds();
    ;
}

void WorkTime::ChangeSeconds(int increment)
{
    _seconds += increment;
    ConstrainSeconds();
}

unsigned long WorkTime::GetSeconds()
{
    return _seconds;
}
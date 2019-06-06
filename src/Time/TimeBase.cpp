#include "TimeBase.h";
 TimeBase::TimeBase(long hours, long minutes){
     _hours = hours;
     _minutes = minutes;
 }
void TimeBase::ConstrainHours(){
    if (_hours > 23)
        _hours = 0;
    if (_hours < 0)
        _hours = 23;
}

void TimeBase::ConstrainMinutes(){
     if (_minutes > 59)
        _minutes = 0;
    if (_minutes < 0)
        _minutes = 59;
}

unsigned long TimeBase::ToSeconds(){
    auto hoursInSeconds = MyDateTimeConverters::HoursToSeconds(_hours);
    auto minutesInSeconds = MyDateTimeConverters::MinutesToSeconds(_minutes);
    return hoursInSeconds + minutesInSeconds;
}

void TimeBase::UpdateValuesFromSeconds(unsigned long seconds){
    auto hours = MyDateTimeConverters::SecondsToHours(seconds);
    auto secondsWithoutDaysAndHours= seconds - MyDateTimeConverters::HoursToSeconds(hours);
    auto minutes = MyDateTimeConverters::SecondsToMinutes(secondsWithoutDaysAndHours);

    _hours = hours;
    _minutes = minutes;
}

void TimeBase::ChangeHours(int increment){
    _hours += increment;
    ConstrainHours();
}
void TimeBase::ChangeMinutes(int increment){
    _minutes += increment;
    ConstrainMinutes();
}
 unsigned long TimeBase::GetHours(){
     return _hours;
 }
unsigned long TimeBase::GetMinutes(){
     return _minutes;
}
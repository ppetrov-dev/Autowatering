#include "Pump.h"

Pump::Pump(byte pin)
{
    _pin = pin;
}
bool Pump::IsEnabled()
{
    return WorkTimeInSeconds != 0 && PauseTimeInMinutes != 0;
}
void Pump::Init()
{
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, LOW);
}

bool Pump::GetIsWorking()
{
    return _isWorking;
}

void Pump::SwitchOn()
{
    digitalWrite(_pin, HIGH);
    _isWorking = true;

    _isForcedlyStated = !GetIsTimeForWatering();
    _startTimeInMilliseconds = millis();
}
void Pump::SwitchOff()
{
    digitalWrite(_pin, LOW);
    _isWorking = false;

   _stopTimeInMilliseconds = millis();
   _isForcedlyStated= false;
}

bool Pump::GetIsTimeForWatering()
{
    if (!IsEnabled() || _isWorking)
        return false;

    auto currentMilliseconds = millis();
    auto pauseTimeInMilliseconds = Converters::MinutesToMilliseconds(PauseTimeInMinutes);
    auto passedTimeinMilliseconds = currentMilliseconds - _stopTimeInMilliseconds;

    return passedTimeinMilliseconds >= pauseTimeInMilliseconds;
}

bool Pump::GetIsTimeToStopWatering()
{
    if (!_isWorking)
        return false;

    auto currentMilliseconds = millis();
    auto workTimeInMilliseconds = Converters::SecondsToMilliseconds(WorkTimeInSeconds);
    auto passedTimeinMilliseconds = currentMilliseconds - _startTimeInMilliseconds;

    return passedTimeinMilliseconds >= workTimeInMilliseconds;
}

String Pump::ConvertMillisecondsToStringTimeFormat(unsigned long milliseconds)
{
    String stringBuilder;
    auto totalSeconds = Converters::MillisecondsToSeconds(milliseconds);

    auto days = Converters::SecondsToDays(totalSeconds);
    if (days != 0)
    {
        stringBuilder.concat(String(days) + "d");
        totalSeconds -= Converters::DaysToSeconds(days);
    }
    auto hours = Converters::SecondsToHours(totalSeconds);
    if(hours<=9)
        stringBuilder.concat("0");
    stringBuilder.concat(String(hours) + ":");
    totalSeconds -= Converters::HoursToSeconds(hours);
    auto minutes = Converters::SecondsToMinutes(totalSeconds);
    if(minutes<=9)
        stringBuilder.concat("0");
    stringBuilder.concat(String(minutes) + ":");
    totalSeconds -= Converters::HoursToMinutes(minutes);
    auto seconds = totalSeconds;
    if(seconds<=9)
        stringBuilder.concat("0");
    stringBuilder.concat(String(seconds));

    return stringBuilder;
}

String Pump::ConvertMillisecondsToSecondsString(unsigned long milliseconds)
{
    auto seconds = Converters::MillisecondsToSeconds(milliseconds);
    return String(seconds) + " sec";
}

String Pump::GetFormatedStringTime(bool showRemaining)
{
    auto currentMilliseconds = millis();
    auto timeInMilliseconds = _isWorking? _startTimeInMilliseconds: _stopTimeInMilliseconds;
    if(!showRemaining || _isForcedlyStated)
        return ConvertMillisecondsToSecondsString(currentMilliseconds - timeInMilliseconds);

    auto periodTimeInMilliseconds = _isWorking ? Converters::SecondsToMilliseconds(WorkTimeInSeconds) : Converters::MinutesToMilliseconds(PauseTimeInMinutes);
    auto remainingTimeInMilliseconds = periodTimeInMilliseconds + timeInMilliseconds - currentMilliseconds;
    return ConvertMillisecondsToStringTimeFormat(remainingTimeInMilliseconds);
}

String Pump::GetStatus(bool showRemaining)
{
    if (!IsEnabled())
        return "disabled";

    auto formatedStringTime = GetFormatedStringTime(showRemaining);
    return String((_isWorking? "work ": "wait ") + formatedStringTime);
}

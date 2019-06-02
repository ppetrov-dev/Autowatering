#include "Pump.h"

Pump::Pump(byte pin)
{
    _pin = pin;
}
bool Pump::IsAutoWateringEnabled()
{
    return WorkTimeInSeconds != 0 && WaitTimeInMinutes != 0;
}
void Pump::Init(unsigned long forcedlyStartedTimerInSeconds, RelayType relayType)
{
    pinMode(_pin, OUTPUT);
    _relayType = relayType;
    _forcedlyStartedTimerInSeconds = forcedlyStartedTimerInSeconds;
    Stop();
}

bool Pump::GetIsWorking()
{
    return _isWorking;
}

void Pump::Start()
{
    auto stateWhenOn = _relayType == HighLevel ? LOW : HIGH;
    _isWorking = true;
    digitalWrite(_pin, stateWhenOn);
    _startTimeInMilliseconds = millis();
}

void Pump::ForceStart(PumpMode pumpMode)
{
    _pumpMode = pumpMode;
    Start();
}
void Pump::Stop()
{
    auto stateWhenOff = _relayType == HighLevel? HIGH: LOW;
    digitalWrite(_pin, stateWhenOff);
    _isWorking = false;

    _stopTimeInMilliseconds = millis();
    _pumpMode = Normal;
}
bool Pump::IsTimeForWatering()
{
    if (!IsAutoWateringEnabled() || _isWorking)
        return false;

    auto currentMilliseconds = millis();
    auto waitTimeInMilliseconds = Converters::MinutesToMilliseconds(WaitTimeInMinutes);
    auto passedTimeinMilliseconds = currentMilliseconds - _stopTimeInMilliseconds;

    return passedTimeinMilliseconds >= waitTimeInMilliseconds;
}

bool Pump::IsTimeToStopWatering()
{
    if (!_isWorking)
        return false;

    auto currentMilliseconds = millis();
    auto periodTimeInMilliseconds =  GetPeriodTimeInMilliseconds();
    auto passedTimeinMilliseconds = currentMilliseconds - _startTimeInMilliseconds;

    return passedTimeinMilliseconds >= periodTimeInMilliseconds;
}

String Pump::ConvertMillisecondsToStringTimeFormat(unsigned long milliseconds)
{
    String stringBuilder;
    auto totalSeconds = Converters::MillisecondsToSeconds(milliseconds);

    auto days = Converters::SecondsToDays(totalSeconds);
    if (days != 0)
    {
        stringBuilder.concat(String(days) + "d ");
        totalSeconds -= Converters::DaysToSeconds(days);
    }
    auto hours = Converters::SecondsToHours(totalSeconds);
    if (hours <= 9)
        stringBuilder.concat("0");
    stringBuilder.concat(String(hours) + ":");
    totalSeconds -= Converters::HoursToSeconds(hours);
    auto minutes = Converters::SecondsToMinutes(totalSeconds);
    if (minutes <= 9)
        stringBuilder.concat("0");
    stringBuilder.concat(String(minutes) + ":");
    totalSeconds -= Converters::HoursToMinutes(minutes);
    auto seconds = totalSeconds;
    if (seconds <= 9)
        stringBuilder.concat("0");
    stringBuilder.concat(String(seconds));

    return stringBuilder;
}

String Pump::ConvertMillisecondsToSecondsString(unsigned long milliseconds)
{
    auto seconds = Converters::MillisecondsToSeconds(milliseconds);
    return String(seconds) + " sec";
}
unsigned long Pump::GetPeriodTimeInMilliseconds()
{
    unsigned long periodTimeInMilliseconds = 0;
    if (_pumpMode == ForcedlyStartedWithTimer)
        periodTimeInMilliseconds = Converters::SecondsToMilliseconds(_forcedlyStartedTimerInSeconds);
    else if (_isWorking)
        periodTimeInMilliseconds = Converters::SecondsToMilliseconds(WorkTimeInSeconds);
    else
        periodTimeInMilliseconds = Converters::MinutesToMilliseconds(WaitTimeInMinutes);

    return periodTimeInMilliseconds;
}
String Pump::GetFormatedStringTime()
{
    auto currentMilliseconds = millis();
    auto timeInMilliseconds = _isWorking ? _startTimeInMilliseconds : _stopTimeInMilliseconds;
    if (_pumpMode == ForcedlyStarted)
        return ConvertMillisecondsToSecondsString(currentMilliseconds - timeInMilliseconds);

    auto periodTimeInMilliseconds = GetPeriodTimeInMilliseconds();
    auto finalTimeInMilliseconds = periodTimeInMilliseconds + timeInMilliseconds;
    auto remainingTimeInMilliseconds = currentMilliseconds < finalTimeInMilliseconds ? finalTimeInMilliseconds - currentMilliseconds : 0;
    return ConvertMillisecondsToStringTimeFormat(remainingTimeInMilliseconds);
}

String Pump::GetStatus()
{
    if (!IsAutoWateringEnabled() && _pumpMode == Normal)
        return "disabled";

    auto formatedStringTime = GetFormatedStringTime();
    return String((_isWorking ? "working " : "waiting ") + formatedStringTime);
}

void Pump::Tick()
{
    if (IsTimeForWatering())
    {
        Start();
        return;
    }

    if (IsTimeToStopWatering() && _pumpMode != ForcedlyStarted)
    {
        Stop();
        return;
    }
}
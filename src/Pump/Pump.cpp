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
    _offsetInMilliseconds = 0;
    _startTimeInMilliseconds = GetCurrentMilliseconds();
}

void Pump::ForceStart(PumpMode pumpMode)
{
    _pumpMode = pumpMode;
    Start();
}
void Pump::Stop()
{
    auto stateWhenOff = _relayType == HighLevel ? HIGH : LOW;
    digitalWrite(_pin, stateWhenOff);
    _isWorking = false;

    _stopTimeInMilliseconds = GetCurrentMilliseconds();
    _pumpMode = Normal;

     if(_onStoppedCallback!=NULL)
         _onStoppedCallback(this);

}
bool Pump::IsTimeForWatering()
{
    if (!IsAutoWateringEnabled() || _isWorking)
        return false;

    auto currentMilliseconds = GetCurrentMilliseconds();
    auto waitTimeInMilliseconds = MyDateTimeConverters::MinutesToMilliseconds(WaitTimeInMinutes);
    auto passedTimeinMilliseconds = currentMilliseconds - _stopTimeInMilliseconds;

    return passedTimeinMilliseconds >= waitTimeInMilliseconds;
}

bool Pump::IsTimeToStopWatering()
{
    if (!_isWorking)
        return false;

    auto currentMilliseconds = GetCurrentMilliseconds();
    auto periodTimeInMilliseconds = GetPeriodTimeInMilliseconds();
    auto passedTimeinMilliseconds = currentMilliseconds - _startTimeInMilliseconds;

    return passedTimeinMilliseconds >= periodTimeInMilliseconds;
}

String Pump::ConvertMillisecondsToStringTimeFormat(unsigned long milliseconds)
{
    String stringBuilder;
    auto totalSeconds = MyDateTimeConverters::MillisecondsToSeconds(milliseconds);

    auto days = MyDateTimeConverters::SecondsToDays(totalSeconds);
    if (days != 0)
    {
        stringBuilder.concat(String(days) + "d ");
        totalSeconds -= MyDateTimeConverters::DaysToSeconds(days);
    }
    auto hours = MyDateTimeConverters::SecondsToHours(totalSeconds);
    if (hours <= 9)
        stringBuilder.concat("0");
    stringBuilder.concat(String(hours) + ":");
    totalSeconds -= MyDateTimeConverters::HoursToSeconds(hours);
    auto minutes = MyDateTimeConverters::SecondsToMinutes(totalSeconds);
    if (minutes <= 9)
        stringBuilder.concat("0");
    stringBuilder.concat(String(minutes) + ":");
    totalSeconds -= MyDateTimeConverters::HoursToMinutes(minutes);
    auto seconds = totalSeconds;
    if (seconds <= 9)
        stringBuilder.concat("0");
    stringBuilder.concat(String(seconds));

    return stringBuilder;
}

String Pump::ConvertMillisecondsToSecondsString(unsigned long milliseconds)
{
    auto seconds = MyDateTimeConverters::MillisecondsToSeconds(milliseconds);
    return String(seconds) + " sec";
}

unsigned long Pump::GetCurrentMilliseconds()
{
    return millis() + _offsetInMilliseconds;
}

unsigned long Pump::GetPeriodTimeInMilliseconds()
{
    unsigned long periodTimeInMilliseconds = 0;
    if (_pumpMode == ForcedlyStartedWithTimer)
        periodTimeInMilliseconds = MyDateTimeConverters::SecondsToMilliseconds(_forcedlyStartedTimerInSeconds);
    else if (_isWorking)
        periodTimeInMilliseconds = MyDateTimeConverters::SecondsToMilliseconds(WorkTimeInSeconds);
    else
        periodTimeInMilliseconds = MyDateTimeConverters::MinutesToMilliseconds(WaitTimeInMinutes);

    return periodTimeInMilliseconds;
}
String Pump::GetFormatedStringTime()
{
    auto currentMilliseconds = GetCurrentMilliseconds();
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
void Pump::ResetOffsetTime(long timeOffsetInSeconds)
{
    auto offsetInMilliseconds = MyDateTimeConverters::SecondsToMilliseconds(timeOffsetInSeconds);
    _offsetInMilliseconds = offsetInMilliseconds;
}

void Pump::AttachOnStopped(void (*onStoppedCallback)(Pump*)){
    _onStoppedCallback = onStoppedCallback;
}
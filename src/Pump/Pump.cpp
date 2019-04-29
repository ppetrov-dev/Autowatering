#include "Pump.h"

Pump::Pump(byte pin)
{
    _pin = pin;
}
bool Pump::IsAutoWateringEnabled()
{
    return WorkTimeInSeconds != 0 && WaitTimeInMinutes != 0;
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

void Pump::Start()
{
    _isForcedlyStated = !IsTimeForWatering();
    digitalWrite(_pin, HIGH);
    _isWorking = true;
    _startTimeInMilliseconds = millis();
}

void Pump::ForceStart()
{
    _isForcedlyStated = !IsTimeForWatering();
    Start();
}
void Pump::Stop()
{
    digitalWrite(_pin, LOW);
    _isWorking = false;

    _stopTimeInMilliseconds = millis();
}
void Pump::ForceStop()
{
    _isForcedlyStated = false;
    Stop();
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

String Pump::GetFormatedStringTime()
{
    auto currentMilliseconds = millis();
    auto timeInMilliseconds = _isWorking ? _startTimeInMilliseconds : _stopTimeInMilliseconds;
    if (_isForcedlyStated)
        return ConvertMillisecondsToSecondsString(currentMilliseconds - timeInMilliseconds);

    auto periodTimeInMilliseconds = _isWorking ? Converters::SecondsToMilliseconds(WorkTimeInSeconds) : Converters::MinutesToMilliseconds(WaitTimeInMinutes);
    auto finalTimeInMilliseconds = periodTimeInMilliseconds + timeInMilliseconds;
    auto remainingTimeInMilliseconds = currentMilliseconds < finalTimeInMilliseconds? finalTimeInMilliseconds - currentMilliseconds: 0;
    return ConvertMillisecondsToStringTimeFormat(remainingTimeInMilliseconds);
}

String Pump::GetStatus()
{
    if (!IsAutoWateringEnabled() && !_isForcedlyStated)
        return "disabled";

    auto formatedStringTime = GetFormatedStringTime();
    return String((_isWorking ? "working " : "waiting ") + formatedStringTime);
}

void Pump::Tick(){
    if (IsTimeForWatering())
    {
      Start();
      return;
    }

    if (IsTimeToStopWatering() && !_isForcedlyStated)
    {
      Stop();
      return;
    }
}
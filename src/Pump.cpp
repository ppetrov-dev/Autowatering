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

    if (_startTimerOption == WhenPumpIsOn)
        _timerInMilliseconds = millis();
}
void Pump::SwitchOff()
{
    digitalWrite(_pin, LOW);
    _isWorking = false;

    if (_startTimerOption == WhenPumpIsOff)
        _timerInMilliseconds = millis();
}

void Pump::SetStartTimerOption(StartTimerOption startTimerOption)
{
    _startTimerOption = startTimerOption;
}

bool Pump::GetIsTimeForWatering()
{
    if (!IsEnabled() || _isWorking)
        return false;

    auto currentMilliseconds = millis();
    auto pauseTimeInMilliseconds = Converters::MinutesToMilliseconds(PauseTimeInMinutes);
    auto passedTimeinMilliseconds = currentMilliseconds - _timerInMilliseconds;

    return passedTimeinMilliseconds >= pauseTimeInMilliseconds;
}

bool Pump::GetIsTimeToStopWatering()
{
    if (!_isWorking)
        return false;

    auto currentMilliseconds = millis();
    auto workTimeInMilliseconds = Converters::SecondsToMilliseconds(WorkTimeInSeconds);
    auto passedTimeinMilliseconds = currentMilliseconds - _timerInMilliseconds;

    return passedTimeinMilliseconds >= workTimeInMilliseconds;
}
String Pump::GetStatus()
{
    if (!IsEnabled())
        return "disabled";

    if (_isWorking)
        return "working...";
    else
        return "waiting...";
}

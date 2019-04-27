#include "Pump.h"

Pump::Pump(byte pin)
{
    _pin = pin;
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

 void Pump::SetStartTimerOption(StartTimerOption startTimerOption){
        _startTimerOption = startTimerOption;
}

 bool Pump::GetIsTimeForWatering(){
      auto currentMilliseconds = millis();

    if(PauseTimeInMinutes == 0 || _isWorking)
        return false;
       
    auto pauseTimeInMilliseconds = Converters::MinutesToMilliseconds(PauseTimeInMinutes);
    auto passedTimeinMilliseconds = currentMilliseconds - _timerInMilliseconds;
   
   return passedTimeinMilliseconds >= pauseTimeInMilliseconds;
}

bool Pump::GetIsTimeToStopWatering(){
    auto currentMilliseconds = millis();

    if(WorkTimeInMinutes == 0 || !_isWorking)
        return false;
       
    auto workTimeInMilliseconds = Converters::MinutesToMilliseconds(WorkTimeInMinutes);
    auto passedTimeinMilliseconds = currentMilliseconds - _timerInMilliseconds;
   
   return passedTimeinMilliseconds >= workTimeInMilliseconds;
}

#ifndef pumpH
#define pumpH

#include <Arduino.h>
#include "enums.h"
#include "converters.h"

class Pump
{
private:
    byte _pin;
    bool _isWorking;
    unsigned long _timerInMilliseconds;
    StartTimerOption _startTimerOption = WhenPumpIsOff;

   
public:
    unsigned long WorkTimeInSeconds = 30;
    unsigned long PauseTimeInMinutes = 10;

    Pump(byte pin);

    void Init();
    bool GetIsWorking();
    void SwitchOn();
    void SwitchOff();
    void SetStartTimerOption(StartTimerOption startTimerOption);

    bool GetIsTimeForWatering();
    bool GetIsTimeToStopWatering();
};

#endif
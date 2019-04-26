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
    uint32_t _timerInMilliseconds;
    StartTimerOption _startTimerOption = WhenPumpIsOff;

public:
    uint32_t WorkTimeInMinutes = 1;
    uint32_t PauseTimeInMinutes = 10;

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
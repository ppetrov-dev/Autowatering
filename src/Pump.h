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

    uint32_t _workTimeInMinutes = 1;
    uint32_t _pauseTimeInMinutes = 10;
public:

    Pump(byte pin);

    void Init();
    bool GetIsWorking();
    void SwitchOn();
    void SwitchOff();
    void SetStartTimerOption(StartTimerOption startTimerOption);

    bool GetIsTimeForWatering();
    bool GetIsTimeToStopWatering();
    
    uint32_t GetWorkTimeInMinutes();
    void SetWorkTimeInMinutes(uint32_t minutes);
    uint32_t GetPauseTimeInMinutes();
    void SetPauseTimeInMinutes(uint32_t minutes);
};

#endif
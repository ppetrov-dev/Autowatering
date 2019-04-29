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
    bool _isForcedlyStated;
    unsigned long _startTimeInMilliseconds = 0;
    unsigned long _stopTimeInMilliseconds = 0;
    bool IsAutoWateringEnabled();
    bool IsTimeForWatering();
    bool IsTimeToStopWatering();
    String ConvertMillisecondsToSecondsString(unsigned long milliseconds);
    String ConvertMillisecondsToStringTimeFormat(unsigned long milliseconds);
    String GetFormatedStringTime();
    void SwitchOn();
    void SwitchOff();

public:
    unsigned long WorkTimeInSeconds = 45;
    unsigned long PauseTimeInMinutes = 1;

    Pump(byte pin);
    void Init();
    bool GetIsWorking();
    
    void ForceSwitchOn();
    void ForceSwitchOff();

    String GetStatus();
    void Tick();
};

#endif
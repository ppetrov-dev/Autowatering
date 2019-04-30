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
    unsigned long _startTimeInMilliseconds = 0;
    unsigned long _stopTimeInMilliseconds = 0;
    bool IsAutoWateringEnabled();
    bool IsTimeForWatering();
    bool IsTimeToStopWatering();
    String ConvertMillisecondsToSecondsString(unsigned long milliseconds);
    String ConvertMillisecondsToStringTimeFormat(unsigned long milliseconds);
    String GetFormatedStringTime();
    unsigned long GetPeriodTimeInMilliseconds();
    void Start();

    PumpMode _pumpMode = Normal;
    unsigned long _forcedlyStartedTimerInSeconds = 0;
public:
    unsigned long WorkTimeInSeconds = 45;
    unsigned long WaitTimeInMinutes = 1;

    Pump(byte pin);
    void Init(unsigned long forcedlyStartedTimerInSeconds);
    bool GetIsWorking();
    
    void ForceStart(PumpMode pumpMode);
    void Stop();

    String GetStatus();
    void Tick();
};

#endif
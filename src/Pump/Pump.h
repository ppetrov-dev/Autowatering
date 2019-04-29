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
    void Start();
    void Stop();

public:
    unsigned long WorkTimeInSeconds = 45;
    unsigned long WaitTimeInMinutes = 1;

    Pump(byte pin);
    void Init();
    bool GetIsWorking();
    
    void ForceStart();
    void ForceStop();

    String GetStatus();
    void Tick();
};

#endif
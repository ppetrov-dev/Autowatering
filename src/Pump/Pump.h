#ifndef pumpH
#define pumpH

#include <Arduino.h>
#include "enums.h"
#include "MyDateTimeConverters.h"

class Pump
{
private:
    byte _pin;
    RelayType _relayType;
    bool _isWorking;
    void (*_onStoppedCallback)(Pump*) = NULL;
    unsigned long _startTimeInMilliseconds = 0;
    unsigned long _stopTimeInMilliseconds = 0;
    unsigned long _offsetInMilliseconds = 0;
    bool IsAutoWateringEnabled();
    bool IsTimeForWatering();
    bool IsTimeToStopWatering();
    String ConvertMillisecondsToSecondsString(unsigned long milliseconds);
    String ConvertMillisecondsToStringTimeFormat(unsigned long milliseconds);
    String GetFormatedStringTime();
    unsigned long GetPeriodTimeInMilliseconds();
    unsigned long GetCurrentMilliseconds();
    void Start();

    PumpMode _pumpMode = Normal;
    unsigned long _forcedlyStartedTimerInSeconds = 0;
public:
    unsigned long WorkTimeInSeconds = 45;
    unsigned long WaitTimeInMinutes = 1;

    Pump(byte pin);
    void Init(unsigned long forcedlyStartedTimerInSeconds, RelayType relayType);
    bool GetIsWorking();
    void ResetOffsetTime(long timeOffsetInSeconds);
    void AttachOnStopped(void onFinishWateringCallback(Pump*));
    void ForceStart(PumpMode pumpMode);
    void Stop();

    String GetStatus();
    void Tick();
};

#endif
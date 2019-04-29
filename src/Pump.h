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
    bool IsEnabled();
    String ConvertMillisecondsToSecondsString(unsigned long milliseconds);
    String ConvertMillisecondsToStringTimeFormat(unsigned long milliseconds);
    String GetFormatedStringTime(bool showRemaining = true);
public:
    unsigned long WorkTimeInSeconds = 30;
    unsigned long PauseTimeInMinutes = 10;

    Pump(byte pin);

    void Init();
    bool GetIsWorking();
    void SwitchOn();
    void SwitchOff();

    bool GetIsTimeForWatering();
    bool GetIsTimeToStopWatering();

    String GetStatus(bool showRemaining = true);
};

#endif
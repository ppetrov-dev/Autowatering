#ifndef timerH
#define timerH

#include <Arduino.h>

extern "C" {
    typedef void (*timerCallback)(void);
}

class Timer
{
private:
    
    timerCallback _onTickCallback;
    bool _isStarted;
    unsigned long _timeInMilliseconds = 0;
    unsigned long _intervalInMilliseconds = 1000;
public:
    void SetInterval(unsigned long timeoutInMilliseconds);
    void Start();
    void Stop();
    void AttachOnTick(timerCallback callback);
    void Tick();
};

#endif // timerH


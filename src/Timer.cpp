#include "Timer.h"

  void Timer::SetTimeout(unsigned long timeoutInMilliseconds)
  {
      _timeoutInMilliseconds = timeoutInMilliseconds;
  }
  void Timer::Start()
  {
      _timeInMilliseconds = millis();
      _isStarted = true;
  }
  void Timer::Stop()
  {
      _isStarted = false;
      _timeInMilliseconds = 0;
  }
  void Timer::AttachOnTick(timerCallback callback)
  {
      _onTickCallback = callback;
  }
  void Timer::Tick()
  {
      if(!_isStarted || _onTickCallback == NULL
      || _timeInMilliseconds == 0)
        return;

      auto currentMilliseconds = millis();
      
      if(currentMilliseconds < _timeInMilliseconds + _timeoutInMilliseconds)
        return;
    
    _timeInMilliseconds = currentMilliseconds;
    _onTickCallback();
  }
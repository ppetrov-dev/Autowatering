#include "Timer.h"

  void Timer::SetInterval(unsigned long intervalInMilliseconds)
  {
      _intervalInMilliseconds = intervalInMilliseconds;
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
      
      if(currentMilliseconds < _timeInMilliseconds + _intervalInMilliseconds)
        return;
    
    _timeInMilliseconds = currentMilliseconds;
    _onTickCallback();
  }
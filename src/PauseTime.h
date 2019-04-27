#ifndef pauseTimeH
#define pauseTimeH

#include "TimeBase.h"

class PauseTime: public TimeBase
{
private:
   long _days;

   void ConstrainDays();
public:
    PauseTime(long days, long hours, long minutes);
    unsigned long ToSeconds();
    void UpdateValuesFromSeconds(unsigned long seconds);
    void ChangeDays(int increment);
    unsigned long GetDays();
};

#endif
#ifndef waitTimeH
#define waitTimeH

#include "TimeBase.h"

class WaitTime: public TimeBase
{
private:
   long _days;

   void ConstrainDays();
public:
    WaitTime(long days, long hours, long minutes);
    unsigned long ToSeconds();
    void UpdateValuesFromSeconds(unsigned long seconds);
    void ChangeDays(int increment);
    unsigned long GetDays();
};

#endif
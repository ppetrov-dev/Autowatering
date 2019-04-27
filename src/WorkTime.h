#ifndef workTimeH
#define workTimeH

#include "TimeBase.h"

class WorkTime: public TimeBase
{
private:
   long _seconds;

   void ConstrainSeconds();
public:
    WorkTime(long hours, long minutes, long seconds);
    unsigned long ToSeconds();
    void UpdateValuesFromSeconds(unsigned long seconds);
    void ChangeSeconds(int increment);
    unsigned long GetSeconds();
};

#endif


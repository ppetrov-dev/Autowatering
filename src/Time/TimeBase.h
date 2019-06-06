#ifndef timeBaseH
#define timeBaseH

#include "MyDateTimeConverters.h"

class TimeBase
{
private:
    long _hours;
    long _minutes;

protected:
    void ConstrainMinutes();
    void ConstrainHours();
    void UpdateValuesFromSeconds(unsigned long seconds);
    unsigned long ToSeconds();
    TimeBase(long hours, long minutes);

public:
    void ChangeHours(int increment);
    void ChangeMinutes(int increment);
    unsigned long GetHours();
    unsigned long GetMinutes();
};

#endif
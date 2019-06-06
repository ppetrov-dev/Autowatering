#ifndef dataH
#define dataH

#define ULONG_MAX 4294967295

struct Data
{
    unsigned long WaitTimeInMinutes;
    unsigned long WorkTimeInSeconds;

    Data() : WaitTimeInMinutes{ULONG_MAX}, WorkTimeInSeconds{ULONG_MAX}
    {
    }
    Data(unsigned long waitTimeInMinutes, unsigned long workTimeInSeconds)
        : WaitTimeInMinutes{waitTimeInMinutes}, WorkTimeInSeconds{workTimeInSeconds}
    {
    }
};
#endif // !dataH

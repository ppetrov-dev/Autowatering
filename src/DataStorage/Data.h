#ifndef dataH
#define dataH

#define ULONG_MAX 4294967295

struct Data
{
    unsigned long WaitTimeInMinutes;
    unsigned long WorkTimeInSeconds;
    unsigned long LastWateringTimeStampInSeconds;

    Data() : Data(ULONG_MAX, ULONG_MAX, ULONG_MAX)
    {
    }
    Data(unsigned long waitTimeInMinutes, unsigned long workTimeInSeconds, unsigned long lastWateringTimeStampInSeconds)
        : WaitTimeInMinutes{waitTimeInMinutes}, WorkTimeInSeconds{workTimeInSeconds},  LastWateringTimeStampInSeconds{lastWateringTimeStampInSeconds}
    {
    }
};
#endif // !dataH

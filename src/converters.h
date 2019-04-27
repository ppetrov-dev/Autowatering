#ifndef convertersH
#define convertersH

class Converters
{
public:
    static unsigned long DaysToHours(unsigned long days)
    {
        return days * 24;
    }

    static unsigned long HoursToMinutes(unsigned long hours)
    {
        return hours * 60;
    }

    static unsigned long SecondsToMilliseconds(unsigned long seconds)
    {
        return seconds * 1000;
    }

    static unsigned long SecondsToMinutes(unsigned long seconds)
    {
        return seconds / 60;
    }

    static unsigned long SecondsToHour(unsigned long seconds)
    {
        auto minutes = SecondsToMinutes(seconds);
        return minutes * 60;
    }

    static unsigned long SecondsToDays(unsigned long seconds)
    {
        auto hours = SecondsToHour(seconds);
        return hours / 24;
    }
    static unsigned long MinutesToSeconds(unsigned long minutes)
    {
        return minutes * 60;
    }
    static unsigned long DaysToMinutes(unsigned long days)
    {
        auto hours = DaysToHours(days);
        return HoursToMinutes(hours);
    }
    static unsigned long DaysToSeconds(unsigned long days)
    {
        auto hours = DaysToHours(days);
        return HoursToSeconds(hours);
    }
    static unsigned long DaysToMilliseconds(unsigned long days)
    {
        auto hours = DaysToHours(days);
        return HoursToMilliseconds(hours);
    }

    static unsigned long HoursToSeconds(unsigned long hours)
    {
        auto minutes = HoursToMinutes(hours);
        return MinutesToSeconds(minutes);
    }
    static unsigned long HoursToMilliseconds(unsigned long hours)
    {
        auto minutes = HoursToMinutes(hours);
        return MinutesToMilliseconds(minutes);
    }

    static unsigned long MinutesToDays(unsigned long minutes)
    {
        auto seconds = MinutesToSeconds(minutes);
        return SecondsToDays(seconds);
    }
    static unsigned long MinutesToHours(unsigned long minutes)
    {
        auto seconds = MinutesToSeconds(minutes);
        return SecondsToHour(seconds);
    }
    static unsigned long MinutesToMilliseconds(unsigned long minutes)
    {
        auto seconds = MinutesToSeconds(minutes);
        return SecondsToMilliseconds(seconds);
    }
};

#endif
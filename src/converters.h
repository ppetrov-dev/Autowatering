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
    static unsigned long MillisecondsToSeconds(unsigned long milliSeconds)
    {
        return milliSeconds / 1000;
    }
    static unsigned long SecondsToMilliseconds(unsigned long seconds)
    {
        return seconds * 1000;
    }
    static unsigned long SecondsToMinutes(unsigned long seconds)
    {
        return seconds / 60;
    }
    static unsigned long SecondsToHours(unsigned long seconds)
    {
        auto minutes = SecondsToMinutes(seconds);
        return minutes / 60;
    }
    static unsigned long HoursToDays(unsigned long hours)
    {
        return hours / 24;
    }
    static unsigned long MinutesToSeconds(unsigned long minutes)
    {
        return minutes * 60;
    }
    static unsigned long SecondsToDays(unsigned long seconds)
    {
        auto hours = SecondsToHours(seconds);
        return HoursToDays(hours);
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
        return SecondsToHours(seconds);
    }
    static unsigned long MinutesToMilliseconds(unsigned long minutes)
    {
        auto seconds = MinutesToSeconds(minutes);
        return SecondsToMilliseconds(seconds);
    }
    static unsigned long MillisecondsToMinutes(unsigned long milliSeconds)
    {
        auto seconds = MillisecondsToSeconds(milliSeconds);
        return SecondsToMinutes(seconds);
    }
    static unsigned long MillisecondsToHours(unsigned long milliSeconds)
    {
        auto minutes = MillisecondsToMinutes(milliSeconds);
        return MinutesToHours(minutes);
    }
    static unsigned long MillisecondsToDays(unsigned long milliSeconds)
    {
        auto hours = MillisecondsToHours(milliSeconds);
        return HoursToDays(hours);
    }
};

#endif
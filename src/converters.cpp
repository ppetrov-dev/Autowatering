
unsigned long ConvertSecondsToMilliseconds(unsigned long seconds)
{
  return seconds * 1000;
}

unsigned long ConvertMinutesToMilliseconds(unsigned long minutes)
{
    
  unsigned long seconds = minutes * 60;
  return ConvertSecondsToMilliseconds(seconds);
}

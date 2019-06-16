#ifndef autoWateringUltrasonicSensorH
#define autoWateringUltrasonicSensorH

#include <HCSR04.h>
#include "settings.h"

#define LONG_DISTANCE 999.9
class AutoWateringUltrasonicSensor
{
private:
  static const double MinDistanceInCm = (double)3.0;
  static const double MaxDistanceInCm = DISTANCE_TO_BOTTOM_CM;
  static const int MaxCountInArray = 10;
  double _lastValidDistance = LONG_DISTANCE;
  double _filterArray[MaxCountInArray];
  int _currentCountInArray = 0;
  bool _isUnknown = true;
  UltraSonicDistanceSensor _ultraSonicDistanceSensor;

public:
  AutoWateringUltrasonicSensor();
  double GetDistance();
  double GetFullnessInPercents();
  bool GetIsUnkown();
  bool GetIsEmpty();
  void Tick();
};

#endif
#include "AutoWateringUltrasonicSensor.h"

AutoWateringUltrasonicSensor::AutoWateringUltrasonicSensor() : _ultraSonicDistanceSensor(PIN_UltrasonicSensorTrigger, PIN_UltrasonicSensorEcho)
{
}
bool AutoWateringUltrasonicSensor::GetIsUnkown()
{
    return _isUnknown;
}
bool AutoWateringUltrasonicSensor::GetIsEmpty()
{
    if (GetIsUnkown())
        return _lastValidDistance > MaxDistanceInCm;

    return GetDistance() >= MaxDistanceInCm;
}
double AutoWateringUltrasonicSensor::GetDistance()
{
    auto sum = 0.0;
    for (auto i = 0; i < _currentCountInArray; i++)
        sum += _filterArray[i];

    auto avarageDistance = sum / _currentCountInArray;
    return avarageDistance;
}

double AutoWateringUltrasonicSensor::GetFullnessInPercents()
{
    auto distance = GetDistance();

    auto fullnestInPercents = (1 - (distance - MinDistanceInCm) / MaxDistanceInCm) * 100;
    if (fullnestInPercents > 100.0)
        fullnestInPercents = 100.0;
    if (fullnestInPercents <= 0.0)
        fullnestInPercents = 0.0;
    return fullnestInPercents;
}

void AutoWateringUltrasonicSensor::Tick()
{
    auto distance = _ultraSonicDistanceSensor.measureDistanceCm();
    _isUnknown = distance == -1;
    if (GetIsUnkown())
        return;
    _lastValidDistance = distance;
    if (_currentCountInArray < MaxCountInArray)
    {
        _filterArray[_currentCountInArray++] = distance;
        return;
    }
    auto n = MaxCountInArray - 1;
    for (auto i = 0; i < n; i++)
        _filterArray[i] = _filterArray[i + 1];

    _filterArray[n] = distance;
}

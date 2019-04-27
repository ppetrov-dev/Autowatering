#include "AutoPumpLcd.h"

AutoPumpLcd::AutoPumpLcd(byte columnCount, byte rowCount, byte pumpAmount) : _lcd(0x27, columnCount, rowCount)
{
    _pumpAmount = pumpAmount;
    _columnCount = columnCount;
    _rowCount = rowCount;
}

void AutoPumpLcd::SetTimeoutInSeconds(byte seconds)
{
    _timeoutInSeconds = seconds;
}

byte AutoPumpLcd::GetTimeoutInSeconds()
{
    return _timeoutInSeconds;
}

void AutoPumpLcd::SetIsAutoOff(bool isAutoOff)
{
    _isAutoOff = isAutoOff;
}
bool AutoPumpLcd::GetIsAutoOff()
{
    return _isAutoOff;
}

bool AutoPumpLcd::GetIsOn()
{
    return _isOn;
}

void AutoPumpLcd::Clear()
{
    _lcd.clear();
}

LcdCursorPosition AutoPumpLcd::GetCursorPosition()
{
    return _cursorPosition;
}
void AutoPumpLcd::Init()
{
    _lcd.init();
}

void AutoPumpLcd::SwitchOn()
{
    _isOn = true;
    _lcd.backlight();
}
void AutoPumpLcd::SwitchOff()
{
    _isOn = false;
    _lcd.noBacklight();
}

LcdCursorPosition AutoPumpLcd::GetNextCursorPosition()
{
    switch (_cursorPosition)
    {
    case SelectPump:
        return SelectPauseDays;
    case SelectPauseDays:
        return SelectPauseHours;
    case SelectPauseHours:
        return SelectPauseMinutes;
    case SelectPauseMinutes:
        return SelectWorkDays;
    case SelectWorkDays:
        return SelectWorkHours;
    case SelectWorkHours:
        return SelectWorkMinutes;
    case SelectWorkMinutes:
        return SelectPump;
    }
}

LcdCursorPosition AutoPumpLcd::GetPreviousCursorPosition()
{
    switch (_cursorPosition)
    {
    case SelectPump:
        return SelectWorkMinutes;
    case SelectPauseDays:
        return SelectPump;
    case SelectPauseHours:
        return SelectPauseDays;
    case SelectPauseMinutes:
        return SelectPauseHours;
    case SelectWorkDays:
        return SelectPauseMinutes;
    case SelectWorkHours:
        return SelectWorkDays;
    case SelectWorkMinutes:
        return SelectWorkHours;
    }
}

bool AutoPumpLcd::GetIsWatchingPauseStates(){

    switch (_cursorPosition)
    {
        case SelectPump:
        case SelectPauseDays:
        case SelectPauseHours:
        case SelectPauseMinutes:
            return true;
        case SelectWorkDays:
        case SelectWorkHours:
        case SelectWorkMinutes:
            return false;
        default:
            return false;
    }
}
void AutoPumpLcd::MoveToNextCursorPosition()
{
    _cursorPosition = GetNextCursorPosition();
}
void AutoPumpLcd::MoveToPreviousCursorPosition()
{
    _cursorPosition = GetPreviousCursorPosition();
}
void AutoPumpLcd::ConstrainSelectedValues()
{
    if (_selectedMinutes > 59)
        _selectedMinutes = 0;
    if (_selectedMinutes < 0)
        _selectedMinutes = 59;

    if (_selectedHours > 23)
        _selectedHours = 0;
    if (_selectedHours < 0)
        _selectedHours = 23;
    
    if (_selectedDays > 9)
        _selectedDays = 0;
    if (_selectedDays < 0)
        _selectedDays = 9;
}

void AutoPumpLcd::ConstrainSelectedPumpIndex()
{
    if (_selectedPumpIndex > _pumpAmount - 1)
        _selectedPumpIndex = 0;
    if (_selectedPumpIndex < 0)
        _selectedPumpIndex = _pumpAmount - 1;
}

void AutoPumpLcd::PrintArrowAndSetCursor(byte col, byte row)
{
    _lcd.setCursor(4, 0);
    _lcd.print("#");
    _lcd.setCursor(7, 1);
    _lcd.print(" ");
    _lcd.setCursor(10, 1);
    _lcd.print(" ");
    _lcd.setCursor(13, 1);
    _lcd.print(":");
    _lcd.setCursor(col, row);
    _lcd.write(126);
}

int AutoPumpLcd::GetSelectedPumpIndex(){
    return _selectedPumpIndex;
}

void AutoPumpLcd::PrintDataAndUpdateArrowPosition()
{
    switch (_cursorPosition)
    {
    case SelectPump:
        PrintArrowAndSetCursor(4, 0);
        break;
    case SelectPauseDays:
    case SelectWorkDays:
        PrintArrowAndSetCursor(7, 1);
        break;
    case SelectPauseHours:
    case SelectWorkHours:
        PrintArrowAndSetCursor(10, 1);
        break;
    case SelectPauseMinutes:
    case SelectWorkMinutes:
        PrintArrowAndSetCursor(13, 1);
        break;
    }
    _lcd.setCursor(0, 1);

    if (GetIsWatchingPauseStates())
        _lcd.print("pause ");
    else
        _lcd.print("work  ");

    _lcd.setCursor(8, 1);
    _lcd.print(_selectedDays);
    _lcd.print("d");
    _lcd.setCursor(11, 1);
    if (_selectedHours < 10)
        _lcd.print(0);
    _lcd.print(_selectedHours);
    _lcd.setCursor(14, 1);
    if (_selectedMinutes < 10)
        _lcd.print(0);
    _lcd.print(_selectedMinutes);
}

void AutoPumpLcd::PrintSelectedPumpName()
{
    _lcd.setCursor(0, 0);
     char emptyString[_columnCount - 1];
    _lcd.print(emptyString);
    
    _lcd.setCursor(0, 0);
    _lcd.print("Pump#"+ String(_selectedPumpIndex + 1));
}

void AutoPumpLcd::UpdateSelectedValues(int increment)
{
    if (_cursorPosition == SelectPump)
    {
        _selectedPumpIndex += increment;
        ConstrainSelectedPumpIndex();
    }
    else
    {
        switch (_cursorPosition)
        {
        case SelectPauseDays:
        case SelectWorkDays:
            _selectedDays += increment;
            break;
        case SelectPauseHours:
        case SelectWorkHours:
            _selectedHours += increment;
            break;
        case SelectPauseMinutes:
        case SelectWorkMinutes:
            _selectedMinutes += increment;
            break;
        default:
            break;
        }

        ConstrainSelectedValues();
    }
}

unsigned long AutoPumpLcd::ConvertSelectedValuesToSeconds()
{
    auto selectedDaysInSecond = Converters::DaysToSeconds(_selectedDays);
    auto selectedHoursInSecond = Converters::HoursToSeconds(_selectedHours);
    auto selectedMinutesInSecond = Converters::MinutesToSeconds(_selectedMinutes);
    auto result = selectedDaysInSecond + selectedHoursInSecond+ selectedMinutesInSecond;

    return result ;
}

void AutoPumpLcd::UpdateSelectedValuesFromSeconds(unsigned long seconds)
{
    auto days = Converters::SecondsToDays(seconds);
    auto secondsWithoutDays= seconds - Converters::DaysToSeconds(days);
    auto hours = Converters::SecondsToHour(secondsWithoutDays);
    auto secondsWithoutDaysAndHours= secondsWithoutDays - Converters::HoursToSeconds(hours);
    auto minutes = Converters::SecondsToMinutes(secondsWithoutDaysAndHours);

    _selectedDays = days;
    _selectedHours = hours;
    _selectedMinutes = minutes;
}

void AutoPumpLcd::Refresh()
{
    Clear();
    PrintSelectedPumpName();
    PrintDataAndUpdateArrowPosition();
}

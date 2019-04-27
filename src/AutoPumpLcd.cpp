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
    if (_selectedDays > 9)
        _selectedDays = 0;
    if (_selectedDays < 0)
        _selectedDays = 9;

    if (_selectedHours > 59)
        _selectedHours = 0;
    if (_selectedHours < 0)
        _selectedHours = 59;

    if (_selectedMinutes > 59)
        _selectedMinutes = 0;
    if (_selectedMinutes < 0)
        _selectedMinutes = 59;
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
    _lcd.setCursor(0, 0);
    _lcd.print(" ");
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
        PrintArrowAndSetCursor(0, 0);
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
    _lcd.setCursor(1, 0);
     char emptyString[_columnCount - 1];
    _lcd.print(emptyString);
    _lcd.setCursor(1, 0);

    _lcd.print("Pump #"+ String(_selectedPumpIndex + 1));
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

unsigned long AutoPumpLcd::ConvertSelectedValuesToMinutes()
{
    return (_selectedDays * 1440 + _selectedHours * 60 + _selectedMinutes);
}

void AutoPumpLcd::UpdateSelectedValuesFromMinutes(unsigned long minutes)
{
    _selectedDays = floor(minutes / 1440);
    _selectedHours = floor((minutes - _selectedDays * 1440) / 60);
    _selectedMinutes = floor(minutes - _selectedDays * 1440 - _selectedHours * 60);
}

void AutoPumpLcd::Refresh()
{
    Clear();
    PrintSelectedPumpName();
    PrintDataAndUpdateArrowPosition();
}

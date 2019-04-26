#include "AutoPumpLcd.h"

AutoPumpLcd::AutoPumpLcd(uint8_t columnCount, uint8_t rowCount) : _lcd(0x27, columnCount, rowCount)
{
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
    if (_selectedMinutes > 59)
        _selectedMinutes = 0;
    if (_selectedHours > 59)
        _selectedHours = 0;
    if (_selectedMinutes < 0)
        _selectedMinutes = 59;
    if (_selectedHours < 0)
        _selectedHours = 59;
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

bool AutoPumpLcd::GetIsWatchingPauseStates(){
return _cursorPosition < SelectWorkDays;
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
        _lcd.print("PAUSE ");
    else
        _lcd.print("WORK  ");

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
    _lcd.print("                ");
    _lcd.setCursor(1, 0);
    _lcd.print(_pumpNames[_selectedPumpIndex]);
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
        if (_cursorPosition == SelectPauseDays || _cursorPosition == SelectWorkDays)
            _selectedDays += increment;
        else if (_cursorPosition == SelectPauseHours || _cursorPosition == SelectWorkHours)
            _selectedHours += increment;
        else if (_cursorPosition == SelectPauseMinutes || _cursorPosition == SelectWorkMinutes)
            _selectedMinutes += increment;

        ConstrainSelectedValues();
    }
}

uint32_t AutoPumpLcd::ConvertSelectedValuesToMinutes()
{
    return ((long)_selectedDays * 1440 + (long)_selectedHours * 60 + _selectedMinutes);
}

void AutoPumpLcd::UpdateSelectedValuesFromMinutes(uint32_t minutes)
{
    _selectedDays = floor((long)minutes / 1440);
    _selectedHours = floor((minutes - (long)_selectedDays * 1440) / 60);
    _selectedMinutes = floor(minutes - (long)_selectedDays * 1440 - (long)_selectedHours * 60);
}

void AutoPumpLcd::Refresh()
{
    Clear();
    PrintSelectedPumpName();
    PrintDataAndUpdateArrowPosition();
}

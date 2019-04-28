#include "AutoPumpLcd.h"

AutoPumpLcd::AutoPumpLcd(byte columnCount, byte rowCount, byte pumpAmount) : _lcd(0x27, columnCount, rowCount)
{
    _pumpAmount = pumpAmount;
    _columnCount = columnCount;
    _rowCount = rowCount;

}

void AutoPumpLcd::GoToSettings()
{
    SetCursorPosition(SelectBack);
}

void AutoPumpLcd::LeaveSettings()
{
    SetCursorPosition(SelectSettings);
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
        return SelectSettings;
    case SelectSettings:
        return SelectPump;

    case SelectBack:
        return SelectPauseDays;
    case SelectPauseDays:
        return SelectPauseHours;
    case SelectPauseHours:
        return SelectPauseMinutes;
    case SelectPauseMinutes:
        return SelectWorkHours;
    case SelectWorkHours:
        return SelectWorkMinutes;
    case SelectWorkMinutes:
        return SelectWorkSeconds;
    case SelectWorkSeconds:
        return SelectBack;
    }
}

LcdCursorPosition AutoPumpLcd::GetPreviousCursorPosition()
{
    switch (_cursorPosition)
    {
    case SelectPump:
        return SelectSettings;
    case SelectSettings:
        return SelectPump;
    case SelectBack:
        return SelectWorkSeconds;
    case SelectPauseDays:
        return SelectBack;
    case SelectPauseHours:
        return SelectPauseDays;
    case SelectPauseMinutes:
        return SelectPauseHours;
    case SelectWorkHours:
        return SelectPauseMinutes;
    case SelectWorkMinutes:
        return SelectWorkHours;
    case SelectWorkSeconds:
        return SelectWorkMinutes;
    }
}

bool AutoPumpLcd::GetAreSettingsMenuOpened()
{
    switch (_cursorPosition)
    {
    case SelectPump:
    case SelectSettings:
        return false;
    default:
        return true;
    }
}

bool AutoPumpLcd::GetArePauseSettingsOpened()
{
    switch (_cursorPosition)
    {
    case SelectPauseDays:
    case SelectPauseHours:
    case SelectPauseMinutes:
        return true;
    default:
        return false;
    }
}

bool AutoPumpLcd::GetAreWorkSettingsOpened()
{
    switch (_cursorPosition)
    {
    case SelectWorkHours:
    case SelectWorkMinutes:
    case SelectWorkSeconds:
        return true;
    default:
        return false;
    }
}

bool AutoPumpLcd::TryUpdateActivityTimeAndSwitchOnIfNeeded()
{
    _lastActivityTimeInMilliseconds = millis();
    if (!_isOn)
    {
        SwitchOn();
        return false;
    }

    return true;
}

void AutoPumpLcd::SetCursorPosition(LcdCursorPosition cursorPosition)
{
    if (!TryUpdateActivityTimeAndSwitchOnIfNeeded())
        return;

    _cursorPosition = cursorPosition;
    UpdateArrowPosition();
}

void AutoPumpLcd::MoveToNextCursorPosition()
{
    auto cursorPosition = GetNextCursorPosition();
    SetCursorPosition(cursorPosition);
}
void AutoPumpLcd::MoveToPreviousCursorPosition()
{
    auto cursorPosition = GetPreviousCursorPosition();
    SetCursorPosition(cursorPosition);
}

void AutoPumpLcd::ConstrainSelectedPumpIndex()
{
    if (_selectedPumpIndex > _pumpAmount - 1)
        _selectedPumpIndex = 0;
    if (_selectedPumpIndex < 0)
        _selectedPumpIndex = _pumpAmount - 1;
}

int AutoPumpLcd::GetSelectedPumpIndex()
{
    return _selectedPumpIndex;
}
void AutoPumpLcd::ClearRow(byte rowIndex)
{
    _lcd.setCursor(0, rowIndex);
    // char emptyString[_columnCount - 1];
    // _lcd.print(emptyString);
    _lcd.print("                ");
    _lcd.setCursor(0, rowIndex);
}

void AutoPumpLcd::RedrawArrowSpots()
{
    _lcd.setCursor(4, 0);
    _lcd.print("#");
    _lcd.setCursor(7, 0);
    _lcd.print(" ");
    _lcd.setCursor(11, 0);
    _lcd.print(" ");
    _lcd.setCursor(7, 1);
    _lcd.print(" ");
    _lcd.setCursor(10, 1);
    _lcd.print(":");
    _lcd.setCursor(13, 1);
    _lcd.print(":");
}

void AutoPumpLcd::UpdateArrowPosition()
{
    RedrawArrowSpots();

    switch (_cursorPosition)
    {
    case SelectPump:
        _lcd.setCursor(4, 0);
        break;
    case SelectPauseDays:
    case SelectWorkHours:
        _lcd.setCursor(7, 1);
        break;
    case SelectPauseHours:
    case SelectWorkMinutes:
        _lcd.setCursor(10, 1);
        break;
    case SelectPauseMinutes:
    case SelectWorkSeconds:
        _lcd.setCursor(13, 1);
        break;
    case SelectSettings:
        _lcd.setCursor(7, 0);
        break;
    case SelectBack:
        _lcd.setCursor(11, 0);
        break;
    }
    _lcd.write(126);
}

void AutoPumpLcd::PrintSelectedPumpName()
{
    ClearRow(0);
    _lcd.print("Pump#" + String(_selectedPumpIndex + 1));
    _lcd.setCursor(8, 0);
    _lcd.print("settings");
}

void AutoPumpLcd::PrintDataAndUpdateArrowPosition()
{
    // ClearRow(1);

    if (GetArePauseSettingsOpened())
    {
        _lcd.print("pause");

        auto days = _pauseTime.GetDays();
        _lcd.setCursor(8, 1);
        _lcd.print(days);
        _lcd.print("d");
        _lcd.print(" ");

        auto hours = _pauseTime.GetHours();
        _lcd.setCursor(11, 1);
        if (hours < 10)
            _lcd.print(0);
        _lcd.print(hours);

        auto minutes = _pauseTime.GetMinutes();
        _lcd.setCursor(14, 1);
        if (minutes < 10)
            _lcd.print(0);
        _lcd.print(minutes);
    }
    else
    {
        _lcd.print("work ");

        auto hours = _workTime.GetHours();
        _lcd.setCursor(8, 1);
        if (hours < 10)
            _lcd.print(0);
        _lcd.print(hours);

        auto minutes = _workTime.GetMinutes();
        _lcd.setCursor(11, 1);
        if (minutes < 10)
            _lcd.print(0);
        _lcd.print(minutes);

        auto seconds = _workTime.GetSeconds();
        _lcd.setCursor(14, 1);
        if (seconds < 10)
            _lcd.print(0);
        _lcd.print(seconds);
    }

    UpdateArrowPosition();
}

void AutoPumpLcd::AttachOnCursorPositionChanged(callbackFunction newFunction){
    _onCursorPositionChangedCallbackFunc = newFunction;
}

void AutoPumpLcd::UpdateSelectedValues(int increment)
{
    if (!TryUpdateActivityTimeAndSwitchOnIfNeeded())
        return;

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
            _pauseTime.ChangeDays(increment);
            break;
        case SelectWorkHours:
            _workTime.ChangeHours(increment);
            break;
        case SelectPauseHours:
            _pauseTime.ChangeHours(increment);
            break;
        case SelectWorkMinutes:
            _workTime.ChangeMinutes(increment);
            break;
        case SelectPauseMinutes:
            _pauseTime.ChangeMinutes(increment);
            break;
        case SelectWorkSeconds:
            _workTime.ChangeSeconds(increment);
            break;
        default:
            break;
        }
    }
}

unsigned long AutoPumpLcd::ConvertWorkTimeToSeconds()
{
    return _workTime.ToSeconds();
}
unsigned long AutoPumpLcd::ConvertPauseTimeToSeconds()
{
    return _pauseTime.ToSeconds();
}

void AutoPumpLcd::UpdateWorkTimeFromSeconds(unsigned long seconds)
{
    _workTime.UpdateValuesFromSeconds(seconds);
}

void AutoPumpLcd::UpdatePauseTimeFromSeconds(unsigned long seconds)
{
    _pauseTime.UpdateValuesFromSeconds(seconds);
}

void AutoPumpLcd::Refresh()
{
    _lcd.clear();
    PrintSelectedPumpName();
    PrintDataAndUpdateArrowPosition();
    SwitchOn();
}

bool AutoPumpLcd::GetIsLcdTimeoutExpired()
{
    auto timeoutInMilliseconds = Converters::SecondsToMilliseconds(TimeoutInSeconds);
    return millis() - _lastActivityTimeInMilliseconds >= timeoutInMilliseconds;
}

void AutoPumpLcd::Tick()
{
    if (!IsAutoOff || !_isOn || !GetIsLcdTimeoutExpired())
        return;

    SwitchOff();
}

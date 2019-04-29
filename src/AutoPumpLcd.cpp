#include "AutoPumpLcd.h"

AutoPumpLcd::AutoPumpLcd(byte columnCount, byte rowCount) : _lcd(0x27, columnCount, rowCount)
{
    _columnCount = columnCount;
    _rowCount = rowCount;
}

int AutoPumpLcd::GetSelectedPumpIndex()
{
    return _selectedPumpIndex;
}

void AutoPumpLcd::ConstrainSelectedPumpIndex()
{
    if (_selectedPumpIndex > _pumpAmount - 1)
        _selectedPumpIndex = 0;
    if (_selectedPumpIndex < 0)
        _selectedPumpIndex = _pumpAmount - 1;
}
void AutoPumpLcd::Init(byte pumpAmount, AutoPumpState state)
{
    _state = state;
    _pumpAmount = pumpAmount;
    _lcd.init();
    PrintState();
    UpdateActivityTimeAndSwitchOnIfNeeded();
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

void AutoPumpLcd::UpdateActivityTimeAndSwitchOnIfNeeded()
{
    _lastActivityTimeInMilliseconds = millis();
    if (_isOn)
        return;
    SwitchOn();
}

#pragma region Printing Methods

void AutoPumpLcd::ClearRow(byte rowIndex)
{
    _lcd.setCursor(0, rowIndex);
    // char emptyString[_columnCount - 1];
    // _lcd.print(emptyString);
    _lcd.print("                ");
    _lcd.setCursor(0, rowIndex);
}

void AutoPumpLcd::ReprintArrowSpots()
{
    switch (_state)
    {
    case SelectPumpState:
    case SelectSettingsState:
        _lcd.setCursor(4, 0);
        _lcd.print("#");
        _lcd.setCursor(7, 0);
        _lcd.print(" ");
        break;
    case SelectBackState:
    case SelectWorkHoursState:
    case SelectWorkMinutesState:
    case SelectWorkSecondsState:
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
        break;
    case SelectPauseDaysState:
    case SelectPauseHoursState:
    case SelectPauseMinutesState:
        _lcd.setCursor(7, 0);
        _lcd.print(" ");
        _lcd.setCursor(11, 0);
        _lcd.print(" ");
        _lcd.setCursor(7, 1);
        _lcd.print(" ");
        _lcd.setCursor(10, 1);
        _lcd.print(" ");
        _lcd.setCursor(13, 1);
        _lcd.print(":");
        break;
    default:
        break;
    }
}

void AutoPumpLcd::PrintArrowPosition()
{
    ReprintArrowSpots();

    switch (_state)
    {
    case SelectPumpState:
        _lcd.setCursor(4, 0);
        break;
    case SelectSettingsState:
        _lcd.setCursor(7, 0);
        break;
    case SelectBackState:
        _lcd.setCursor(11, 0);
        break;
    case SelectPauseDaysState:
    case SelectWorkHoursState:
        _lcd.setCursor(7, 1);
        break;
    case SelectPauseHoursState:
    case SelectWorkMinutesState:
        _lcd.setCursor(10, 1);
        break;
    case SelectPauseMinutesState:
    case SelectWorkSecondsState:
        _lcd.setCursor(13, 1);
        break;
    }
    _lcd.write(126);
}

void AutoPumpLcd::PrintPumpName()
{
    _lcd.setCursor(0, 0);
    _lcd.print("Pump");
    _lcd.setCursor(5, 0);
    _lcd.print(String(_selectedPumpIndex + 1));
}

void AutoPumpLcd::PrintSettings()
{
    _lcd.setCursor(8, 0);
    _lcd.print("settings");
}

void AutoPumpLcd::PrintBack()
{
    _lcd.setCursor(8, 0);
    _lcd.print("   ");
    _lcd.setCursor(12, 0);
    _lcd.print("back");
}
void AutoPumpLcd::PrintWorkHours()
{
    auto hours = _workTime.GetHours();
    _lcd.setCursor(8, 1);
    if (hours < 10)
        _lcd.print(0);
    _lcd.print(hours);
}
void AutoPumpLcd::PrintWorkMinutes()
{
    auto minutes = _workTime.GetMinutes();
    _lcd.setCursor(11, 1);
    if (minutes < 10)
        _lcd.print(0);
    _lcd.print(minutes);
}
void AutoPumpLcd::PrintWorkSeconds()
{
    auto seconds = _workTime.GetSeconds();
    _lcd.setCursor(14, 1);
    if (seconds < 10)
        _lcd.print(0);
    _lcd.print(seconds);
}

void AutoPumpLcd::PrintPauseDays()
{
    auto days = _pauseTime.GetDays();
    _lcd.setCursor(8, 1);
    _lcd.print(days);
    _lcd.print("d");
    _lcd.print(" ");
}
void AutoPumpLcd::PrintPauseHours()
{
    auto hours = _pauseTime.GetHours();
    _lcd.setCursor(11, 1);
    if (hours < 10)
        _lcd.print(0);
    _lcd.print(hours);
}
void AutoPumpLcd::PrintPauseMinutes()
{
    auto minutes = _pauseTime.GetMinutes();
    _lcd.setCursor(14, 1);
    if (minutes < 10)
        _lcd.print(0);
    _lcd.print(minutes);
}

void AutoPumpLcd::PrintWorkRow()
{
    _lcd.setCursor(0, 1);
    _lcd.print("work ");
    PrintWorkHours();
    PrintWorkMinutes();
    PrintWorkSeconds();
}

void AutoPumpLcd::PrintPauseRow()
{
    _lcd.setCursor(0, 1);
    _lcd.print("pause");

    PrintPauseDays();
    PrintPauseHours();
    PrintPauseMinutes();
}
void AutoPumpLcd::PrintState()
{
     switch (_state)
    {
    case SelectSettingsState:
    case SelectPumpState:
        PrintPumpName();
        PrintSettings();
        break;
    case SelectPauseDaysState:
    case SelectPauseHoursState:
    case SelectPauseMinutesState:
        PrintPumpName();
        PrintBack();
        PrintPauseRow();
        break;
    case SelectBackState:
        ClearRow(1);
    case SelectWorkHoursState:
    case SelectWorkMinutesState:
    case SelectWorkSecondsState:
        PrintPumpName();
        PrintBack();
        PrintWorkRow();
        break;
    default:
        break;
    }
    
    PrintArrowPosition();
}
void AutoPumpLcd::PrintToRow(byte rowIndex, String text)
{
    if( rowIndex >= _rowCount)
        return;
    
     ClearRow(rowIndex);
    _lcd.setCursor(0, rowIndex);
    _lcd.print(text);
}


#pragma endregion

#pragma region Attach Methods

void AutoPumpLcd::AttachOnSelectedPumpChanged(autoPumpLcdCallback callback)
{
    _onSelectedPumpChangedCallback = callback;
}
#pragma endregion

void AutoPumpLcd::UpdateStateIfNeeded(AutoPumpState newState)
{
    if (_state == newState)
        return;

    _state = newState;

    PrintState();
    UpdateActivityTimeAndSwitchOnIfNeeded();
}

void AutoPumpLcd::SetSelectedPumpIndex(int newPumpIndex)
{
    if (_selectedPumpIndex == newPumpIndex)
        return;

    _selectedPumpIndex = newPumpIndex;

    ConstrainSelectedPumpIndex();
    PrintPumpName();

    if (!_onSelectedPumpChangedCallback)
        return;
    _onSelectedPumpChangedCallback();
}

void AutoPumpLcd::UpdateSelectedValues(int increment)
{
    if (_state == SelectPumpState)
        SetSelectedPumpIndex(_selectedPumpIndex + increment);
    else
        {
            switch (_state)
            {
            case SelectPauseDaysState:
                _pauseTime.ChangeDays(increment);
                PrintPauseDays();
                break;
            case SelectPauseHoursState:
                _pauseTime.ChangeHours(increment);
                PrintPauseHours();
                break;
            case SelectPauseMinutesState:
                _pauseTime.ChangeMinutes(increment);
                PrintPauseMinutes();
                break;
            case SelectWorkHoursState:
                _workTime.ChangeHours(increment);
                PrintWorkHours();
                break;
            case SelectWorkMinutesState:
                _workTime.ChangeMinutes(increment);
                PrintWorkMinutes();
                break;
            case SelectWorkSecondsState:
                _workTime.ChangeSeconds(increment);
                PrintWorkSeconds();
                break;
            default:
                break;
            }
        }

    UpdateActivityTimeAndSwitchOnIfNeeded();
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

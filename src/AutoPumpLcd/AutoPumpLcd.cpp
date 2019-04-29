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
}

void AutoPumpLcd::Refresh()
{
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
    case SelectWaitDaysState:
    case SelectWaitHoursState:
    case SelectWaitMinutesState:
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
    case SelectWaitDaysState:
    case SelectWorkHoursState:
        _lcd.setCursor(7, 1);
        break;
    case SelectWaitHoursState:
    case SelectWorkMinutesState:
        _lcd.setCursor(10, 1);
        break;
    case SelectWaitMinutesState:
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

void AutoPumpLcd::PrintWaitDays()
{
    auto days = _waitTime.GetDays();
    _lcd.setCursor(8, 1);
    _lcd.print(days);
    _lcd.print("d");
    _lcd.print(" ");
}
void AutoPumpLcd::PrintWaitHours()
{
    auto hours = _waitTime.GetHours();
    _lcd.setCursor(11, 1);
    if (hours < 10)
        _lcd.print(0);
    _lcd.print(hours);
}
void AutoPumpLcd::PrintWaitMinutes()
{
    auto minutes = _waitTime.GetMinutes();
    _lcd.setCursor(14, 1);
    if (minutes < 10)
        _lcd.print(0);
    _lcd.print(minutes);
}

void AutoPumpLcd::PrintWorkRow()
{
    _lcd.setCursor(0, 1);
    _lcd.print("works");
    PrintWorkHours();
    PrintWorkMinutes();
    PrintWorkSeconds();
}

void AutoPumpLcd::PrintWaitRow()
{
    _lcd.setCursor(0, 1);
    _lcd.print("waits");

    PrintWaitDays();
    PrintWaitHours();
    PrintWaitMinutes();
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
    case SelectWaitDaysState:
    case SelectWaitHoursState:
    case SelectWaitMinutesState:
        PrintPumpName();
        PrintBack();
        PrintWaitRow();
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
String AutoPumpLcd::ConstrainInputText(String inputedText)
{
    auto lenght = inputedText.length();

    if (lenght < _columnCount)
    {
        auto indexOfFirstSpace = inputedText.indexOf(" ");
        if (indexOfFirstSpace != -1)
        {
            auto firstSubstring = inputedText.substring(0, indexOfFirstSpace);
            auto secondSubstring = inputedText.substring(indexOfFirstSpace, lenght);
            
            for (byte i = 0; i < _columnCount - lenght; i++)
                firstSubstring.concat(" ");
            inputedText = firstSubstring + secondSubstring;
        }
        else{
             for (byte i = 0; i < _columnCount - lenght; i++)
                inputedText.concat(" ");
        }
    }
    return inputedText;
}
void AutoPumpLcd::PrintOnRow(byte rowIndex, String text)
{
    if (rowIndex >= _rowCount)
        return;
    text = ConstrainInputText(text);
    _lcd.setCursor(0, rowIndex);
    _lcd.print(text);
}

#pragma endregion

void AutoPumpLcd::AttachOnSelectedPumpChanged(autoPumpLcdCallback callback)
{
    _onSelectedPumpChangedCallback = callback;
}

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
        case SelectWaitDaysState:
            _waitTime.ChangeDays(increment);
            PrintWaitDays();
            break;
        case SelectWaitHoursState:
            _waitTime.ChangeHours(increment);
            PrintWaitHours();
            break;
        case SelectWaitMinutesState:
            _waitTime.ChangeMinutes(increment);
            PrintWaitMinutes();
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
unsigned long AutoPumpLcd::ConvertWaitTimeToSeconds()
{
    return _waitTime.ToSeconds();
}

void AutoPumpLcd::UpdateWorkTimeFromSeconds(unsigned long seconds)
{
    _workTime.UpdateValuesFromSeconds(seconds);
}

void AutoPumpLcd::UpdateWaitTimeFromSeconds(unsigned long seconds)
{
    _waitTime.UpdateValuesFromSeconds(seconds);
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

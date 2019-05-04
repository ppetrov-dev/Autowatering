#include "AutoWateringLcd.h"

AutoWateringLcd::AutoWateringLcd(byte columnCount, byte rowCount): _lcd(0x27, columnCount, rowCount)
{
    _columnCount = columnCount;
    _rowCount = rowCount;
}

int AutoWateringLcd::GetSelectedPumpIndex()
{
    return _selectedPumpIndex;
}

void AutoWateringLcd::ConstrainSelectedPumpIndex()
{
    if (_selectedPumpIndex > _pumpAmount - 1)
        _selectedPumpIndex = 0;
    if (_selectedPumpIndex < 0)
        _selectedPumpIndex = _pumpAmount - 1;
}
void AutoWateringLcd::Init(byte pumpAmount)
{
    _pumpAmount = pumpAmount;
     _lcd.init();
}

void AutoWateringLcd::Refresh(AutoWateringState state)
{
    PrintState(state);
    UpdateActivityTimeAndSwitchOnIfNeeded();
}

void AutoWateringLcd::SwitchOn()
{
    _isOn = true;
    _lcd.backlight();
}

void AutoWateringLcd::SwitchOff()
{
    _isOn = false;
    _lcd.noBacklight();
}

void AutoWateringLcd::UpdateActivityTimeAndSwitchOnIfNeeded()
{
    _lastActivityTimeInMilliseconds = millis();
    if (_isOn)
        return;
    SwitchOn();
}

#pragma region Printing Methods

void AutoWateringLcd::ClearRow(byte rowIndex)
{
    _lcd.setCursor(0, rowIndex);
    _lcd.print("                ");
    _lcd.setCursor(0, rowIndex);
}

void AutoWateringLcd::ReprintArrowSpots(AutoWateringState state)
{
    switch (state)
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

void AutoWateringLcd::PrintArrowPosition(AutoWateringState state)
{
    ReprintArrowSpots(state);

    switch (state)
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

void AutoWateringLcd::PrintPumpName()
{
    _lcd.setCursor(0, 0);
    _lcd.print("Pump");
    _lcd.setCursor(5, 0);
    _lcd.print(String(_selectedPumpIndex + 1));
}

void AutoWateringLcd::PrintSettings()
{
    _lcd.setCursor(8, 0);
    _lcd.print("settings");
}

void AutoWateringLcd::PrintBack()
{
    _lcd.setCursor(8, 0);
    _lcd.print("   ");
    _lcd.setCursor(12, 0);
    _lcd.print("back");
}
void AutoWateringLcd::PrintWorkHours()
{
    auto hours = _workTime.GetHours();
    _lcd.setCursor(8, 1);
    if (hours < 10)
        _lcd.print(0);
    _lcd.print(hours);
}
void AutoWateringLcd::PrintWorkMinutes()
{
    auto minutes = _workTime.GetMinutes();
    _lcd.setCursor(11, 1);
    if (minutes < 10)
        _lcd.print(0);
    _lcd.print(minutes);
}
void AutoWateringLcd::PrintWorkSeconds()
{
    auto seconds = _workTime.GetSeconds();
    _lcd.setCursor(14, 1);
    if (seconds < 10)
        _lcd.print(0);
    _lcd.print(seconds);
}

void AutoWateringLcd::PrintWaitDays()
{
    auto days = _waitTime.GetDays();
    _lcd.setCursor(8, 1);
    _lcd.print(days);
    _lcd.print("d");
    _lcd.print(" ");
}
void AutoWateringLcd::PrintWaitHours()
{
    auto hours = _waitTime.GetHours();
    _lcd.setCursor(11, 1);
    if (hours < 10)
        _lcd.print(0);
    _lcd.print(hours);
}
void AutoWateringLcd::PrintWaitMinutes()
{
    auto minutes = _waitTime.GetMinutes();
    _lcd.setCursor(14, 1);
    if (minutes < 10)
        _lcd.print(0);
    _lcd.print(minutes);
}

void AutoWateringLcd::PrintWorkRow()
{
    _lcd.setCursor(0, 1);
    _lcd.print("works");
    PrintWorkHours();
    PrintWorkMinutes();
    PrintWorkSeconds();
}

void AutoWateringLcd::PrintWaitRow()
{
    _lcd.setCursor(0, 1);
    _lcd.print("waits");

    PrintWaitDays();
    PrintWaitHours();
    PrintWaitMinutes();
}
void AutoWateringLcd::PrintState(AutoWateringState state)
{
    switch (state)
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

    PrintArrowPosition(state);
}
String AutoWateringLcd::ConstrainInputText(String inputedText)
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
void AutoWateringLcd::PrintOnRow(byte rowIndex, String text)
{
    if (rowIndex >= _rowCount)
        return;
    text = ConstrainInputText(text);
    _lcd.setCursor(0, rowIndex);
    _lcd.print(text);
}

#pragma endregion

void AutoWateringLcd::AttachOnSelectedPumpChanged(autoWateringLcdCallback callback)
{
    _onSelectedPumpChangedCallback = callback;
}

void AutoWateringLcd::UpdateState(AutoWateringState newState)
{
    PrintState(newState);
    UpdateActivityTimeAndSwitchOnIfNeeded();
}

void AutoWateringLcd::SetSelectedPumpIndex(int newPumpIndex)
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

void AutoWateringLcd::UpdateSelectedValues(AutoWateringState state, int increment)
{
    if (state == SelectPumpState)
        SetSelectedPumpIndex(_selectedPumpIndex + increment);
    else
    {
        switch (state)
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

unsigned long AutoWateringLcd::ConvertWorkTimeToSeconds()
{
    return _workTime.ToSeconds();
}
unsigned long AutoWateringLcd::ConvertWaitTimeToSeconds()
{
    return _waitTime.ToSeconds();
}

void AutoWateringLcd::UpdateWorkTimeFromSeconds(unsigned long seconds)
{
    _workTime.UpdateValuesFromSeconds(seconds);
}

void AutoWateringLcd::UpdateWaitTimeFromSeconds(unsigned long seconds)
{
    _waitTime.UpdateValuesFromSeconds(seconds);
}

bool AutoWateringLcd::GetIsLcdTimeoutExpired()
{
    auto timeoutInMilliseconds = Converters::SecondsToMilliseconds(TimeoutInSeconds);
    return millis() - _lastActivityTimeInMilliseconds >= timeoutInMilliseconds;
}

void AutoWateringLcd::Tick()
{
    if (!IsAutoOff || !_isOn || !GetIsLcdTimeoutExpired())
        return;

    SwitchOff();
}

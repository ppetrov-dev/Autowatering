#ifndef arduinoH
#define arduinoH
#include <Arduino.h>
#endif

#ifndef autoPumpLcdH
#define autoPumpLcdH

#include "LCD_1602_RUS.h"
#include "enums.h"
#include "converters.h"
#include "WorkTime.h"
#include "PauseTime.h"

extern "C"
{
    typedef void (*callbackFunction)(void);
}

class AutoPumpLcd
{
private:
    unsigned long _lastActivityTimeInMilliseconds;
    LCD_1602_RUS _lcd;
    byte _columnCount;
    byte _rowCount;
    byte _pumpAmount = 1;
    bool _isOn;
    WorkTime _workTime = WorkTime(0, 0, 30);
    PauseTime _pauseTime = PauseTime(1, 0, 0);
    int _selectedPumpIndex = 0;

    enum LcdCursorPosition _cursorPosition = SelectPump;
    callbackFunction _onCursorPositionChangedCallbackFunc;

    void ConstrainSelectedPumpIndex();
    void SetCursorPosition(LcdCursorPosition cursorPosition);
    LcdCursorPosition GetNextCursorPosition();
    LcdCursorPosition GetPreviousCursorPosition();

    void ClearRow(byte rowIndex);
    void UpdateArrowPosition();
    void RedrawArrowSpots();
    bool GetIsLcdTimeoutExpired();
    bool TryUpdateActivityTimeAndSwitchOnIfNeeded();
    void SwitchOn();
    void SwitchOff();
public:
    bool IsAutoOff = true;
    byte TimeoutInSeconds = 30;
    AutoPumpLcd(byte columnCount, byte rowCount, byte pumpAmount);
    void Init();
    void AttachOnCursorPositionChanged(callbackFunction newFunction);
    bool GetArePauseSettingsOpened();
    bool GetAreWorkSettingsOpened();
    bool GetAreSettingsMenuOpened();
    int GetSelectedPumpIndex();
    void PrintSelectedPumpName();
    void PrintDataAndUpdateArrowPosition();
    void MoveToNextCursorPosition();
    void MoveToPreviousCursorPosition();
    void UpdateSelectedValues(int increment);
    unsigned long ConvertWorkTimeToSeconds();
    unsigned long ConvertPauseTimeToSeconds();
    void UpdateWorkTimeFromSeconds(unsigned long seconds);
    void UpdatePauseTimeFromSeconds(unsigned long seconds);
    LcdCursorPosition GetCursorPosition();
    void Refresh();
    void GoToSettings();
    void LeaveSettings();
    void Tick();
};
#endif
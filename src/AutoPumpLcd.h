#ifndef autoPumpLcdH
#define autoPumpLcdH

#include "LCD_1602_RUS.h"
#include "enums.h"
#include "converters.h"
#include "WorkTime.h"
#include "PauseTime.h"

class AutoPumpLcd
{
private:
    LCD_1602_RUS _lcd;
    byte _columnCount;
    byte _rowCount;
    bool _isOn;
    bool _isAutoOff = true;
    byte _timeoutInSeconds = 30;
    enum LcdCursorPosition _cursorPosition;
    int _selectedPumpIndex = 0;

    WorkTime _workTime = WorkTime(0, 0, 30);
    PauseTime _pauseTime = PauseTime(1, 0, 0);

    byte _pumpAmount = 1;

    void ConstrainSelectedPumpIndex();
    void ClearRow(byte rowIndex);
    void UpdateArrowPosition();
    void RedrawArrowSpots();
    void SetCursorPosition(LcdCursorPosition cursorPosition);
    LcdCursorPosition GetNextCursorPosition();
    LcdCursorPosition GetPreviousCursorPosition();

public:
    AutoPumpLcd(byte columnCount, byte rowCount, byte pumpAmount);
    void Init();
    void SetTimeoutInSeconds(byte seconds);
    byte GetTimeoutInSeconds();
    bool GetIsWatchingPauseStates();
    int GetSelectedPumpIndex();
    void SetIsAutoOff(bool isAutoOff);
    bool GetIsAutoOff();
    bool GetIsOn();
    void SwitchOn();
    void SwitchOff();
    void PrintSelectedPumpName();
    void PrintDataAndUpdateArrowPosition();
    void MoveToNextCursorPosition();
    void MoveToPreviousCursorPosition();
    void UpdateSelectedValues(int increment);
    unsigned long ConvertSelectedValuesToSeconds();
    void UpdateSelectedValuesFromSeconds(unsigned long seconds);
    LcdCursorPosition GetCursorPosition();
    void Refresh();
    void Clear();
};
#endif
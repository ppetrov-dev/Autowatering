#ifndef autoPumpLcdH
#define autoPumpLcdH

#include "LCD_1602_RUS.h"
#include "enums.h"
#include "converters.h"

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
    long _selectedDays = 0;
    long _selectedHours = 0;
    long _selectedMinutes = 10;
    byte _pumpAmount = 1;

    void PrintArrowAndSetCursor(byte col, byte row);
    void ConstrainSelectedValues();
    void ConstrainSelectedPumpIndex();

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
    unsigned long ConvertSelectedValuesToMinutes();
    void UpdateSelectedValuesFromMinutes(unsigned long minutes);
    LcdCursorPosition GetCursorPosition();
    void Refresh();
    void Clear();
};
#endif
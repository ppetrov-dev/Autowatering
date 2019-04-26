#ifndef autoPumpLcdH
#define autoPumpLcdH

#include "LCD_1602_RUS.h"
#include "enums.h"

class AutoPumpLcd
{
private:
    LCD_1602_RUS _lcd;
    uint8_t _columnCount;
    uint8_t _rowCount;
    bool _isOn;
    bool _isAutoOff = true;
    byte _timeoutInSeconds = 30;
    enum LcdCursorPosition _cursorPosition;
    int8_t _selectedPumpIndex = 0;
    int8_t _selectedDays = 0;
    int8_t _selectedHours = 0;
    int8_t _selectedMinutes = 10;
    String _pumpNames[2] = {"Pump 1",
                            "Pump 2"};
    byte _pumpAmount = 2;

    void PrintArrowAndSetCursor(byte col, byte row);
    void ConstrainSelectedValues();
    void ConstrainSelectedPumpIndex();

    LcdCursorPosition GetNextCursorPosition();
    LcdCursorPosition GetPreviousCursorPosition();

public:
    AutoPumpLcd(uint8_t columnCount, uint8_t rowCount);
    void Init();
    void SetTimeoutInSeconds(byte seconds);
    byte GetTimeoutInSeconds();
    bool GetIsWatchingPauseStates();
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
    uint32_t ConvertSelectedValuesToMinutes();
    void UpdateSelectedValuesFromMinutes(uint32_t minutes);
    LcdCursorPosition GetCursorPosition();
    void Refresh();
    void Clear();
};
#endif
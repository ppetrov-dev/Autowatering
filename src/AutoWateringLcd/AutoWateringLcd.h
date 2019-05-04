#ifndef autoWateringLcdH
#define autoWateringLcdH

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include "enums.h"
#include "converters.h"
#include "Time/WorkTime.h"
#include "Time/WaitTime.h"

extern "C" {
    typedef void (*autoWateringLcdCallback)(void);
}

class AutoWateringLcd
{
private:
    autoWateringLcdCallback _onSelectedPumpChangedCallback;

    unsigned long _lastActivityTimeInMilliseconds;
    LiquidCrystal_I2C _lcd;
    byte _columnCount;
    byte _rowCount;
    byte _pumpAmount = 1;
    bool _isOn;
    WorkTime _workTime = WorkTime(0, 0, 30);
    WaitTime _waitTime = WaitTime(1, 0, 0);
    int _selectedPumpIndex = 0;

    String ConstrainInputText(String inputedText);
    void ConstrainSelectedPumpIndex();
    void SetSelectedPumpIndex(int newPumpIndex);
    bool GetIsLcdTimeoutExpired();
    void UpdateActivityTimeAndSwitchOnIfNeeded();
    void SwitchOn();
    void SwitchOff();

    void PrintArrowPosition(AutoWateringState state);
    void ReprintArrowSpots(AutoWateringState state);
    void PrintWaitRow();
    void PrintWorkRow();
    void PrintPumpName();
    void PrintSettings();
    void PrintBack();
    void PrintState(AutoWateringState state);

    void PrintWaitDays();
    void PrintWaitHours();
    void PrintWaitMinutes();
    void PrintWorkHours();
    void PrintWorkMinutes();
    void PrintWorkSeconds();
public:
    bool IsAutoOff = true;
    byte TimeoutInSeconds = 30;
    
    AutoWateringLcd(byte columnCount, byte rowCount);
    void Init(byte pumpAmount);
    void Refresh(AutoWateringState state);
    int GetSelectedPumpIndex();
    void UpdateState(AutoWateringState newState);
    void UpdateSelectedValues(AutoWateringState state, int increment);
    
    void PrintOnRow(byte rowIndex, String text);
    void ClearRow(byte rowIndex);
    
    unsigned long ConvertWorkTimeToSeconds();
    unsigned long ConvertWaitTimeToSeconds();
    void UpdateWorkTimeFromSeconds(unsigned long seconds);
    void UpdateWaitTimeFromSeconds(unsigned long seconds);

    void AttachOnSelectedPumpChanged(autoWateringLcdCallback callback);
    
    void Tick();
};
#endif
#ifndef autoPumpLcdH
#define autoPumpLcdH

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include "enums.h"
#include "converters.h"
#include "Time/WorkTime.h"
#include "Time/WaitTime.h"

extern "C" {
    typedef void (*autoPumpLcdCallback)(void);
}

class AutoPumpLcd
{
private:
    autoPumpLcdCallback _onSelectedPumpChangedCallback;

    unsigned long _lastActivityTimeInMilliseconds;
    LiquidCrystal_I2C _lcd;
    byte _columnCount;
    byte _rowCount;
    byte _pumpAmount = 1;
    bool _isOn;
    WorkTime _workTime = WorkTime(0, 0, 30);
    WaitTime _waitTime = WaitTime(1, 0, 0);
    int _selectedPumpIndex = 0;

    enum AutoPumpState _state;
    String ConstrainInputText(String inputedText);
    void ConstrainSelectedPumpIndex();
    void SetSelectedPumpIndex(int newPumpIndex);
    bool GetIsLcdTimeoutExpired();
    void UpdateActivityTimeAndSwitchOnIfNeeded();
    void SwitchOn();
    void SwitchOff();

    void PrintArrowPosition();
    void ReprintArrowSpots();
    void PrintWaitRow();
    void PrintWorkRow();
    void PrintPumpName();
    void PrintSettings();
    void PrintBack();
    void PrintState();

    void PrintWaitDays();
    void PrintWaitHours();
    void PrintWaitMinutes();
    void PrintWorkHours();
    void PrintWorkMinutes();
    void PrintWorkSeconds();
public:
    bool IsAutoOff = true;
    byte TimeoutInSeconds = 30;
    
    AutoPumpLcd(byte columnCount, byte rowCount);
    void Init(byte pumpAmount, AutoPumpState state);
    int GetSelectedPumpIndex();
    void UpdateStateIfNeeded(AutoPumpState newState);
    void UpdateSelectedValues(int increment);
    
    void PrintOnRow(byte rowIndex, String text);
    void ClearRow(byte rowIndex);
    
    unsigned long ConvertWorkTimeToSeconds();
    unsigned long ConvertWaitTimeToSeconds();
    void UpdateWorkTimeFromSeconds(unsigned long seconds);
    void UpdateWaitTimeFromSeconds(unsigned long seconds);

    void AttachOnSelectedPumpChanged(autoPumpLcdCallback callback);
    
    void Tick();
};
#endif
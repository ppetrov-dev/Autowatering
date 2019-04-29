#include "OneButton.h"
#include <EEPROM.h>
#include "AutoPumpEncoder/AutoPumpEncoder.h"
#include "AutoPumpLcd/AutoPumpLcd.h"
#include "AutoPumpStateMachine/AutoPumpStateMachine.h"
#include "Pump/Pump.h"
#include "settings.h"
#include "enums.h"
#include "converters.h"
#include "Timer/Timer.h"

AutoPumpStateMachine _autoPumpStateMachine;
AutoPumpEncoder _autoPumpEncoder = AutoPumpEncoder(PIN_EncoderClk, PIN_EncoderDt, PIN_EncoderSw);

OneButton _pumpButton1 = OneButton(PIN_Button1, true, true);
Pump _pump1 = Pump(PIN_Pump1);
OneButton _pumpButton2 = OneButton(PIN_Button2, true, true);
Pump _pump2 = Pump(PIN_Pump2);
AutoPumpLcd _autoPumpLcd = AutoPumpLcd(16, 2);
Timer _timer;

Pump *_pumps[PUPM_AMOUNT];

bool _isWatering;

void UpdateDataInMemoryForSelectedPump()
{
  // EEPROM.updateLong(8 * _selectedPumpNumber, _pumpWaitTimesInMinutes[_selectedPumpNumber]);
  // EEPROM.updateLong(8 * _selectedPumpNumber + 4, _pumpWorkTimesInMinutes[_selectedPumpNumber]);
}

void UpdateSelectedValuesToSelectedPump(unsigned long waitTimeInSeconds, unsigned long workTimeInSeconds)
{
  auto pumpIndex = _autoPumpLcd.GetSelectedPumpIndex();
  auto pump = _pumps[pumpIndex];

  pump->WaitTimeInMinutes = Converters::SecondsToMinutes(waitTimeInSeconds);
  pump->WorkTimeInSeconds = workTimeInSeconds;
}

Pump *GetSelectedPump()
{
  auto pumpIndex = _autoPumpLcd.GetSelectedPumpIndex();
  auto pump = _pumps[pumpIndex];
  return pump;
}

void UpdateSelectedValuesFromSelectedPump()
{
  auto pump = GetSelectedPump();
  auto waitTimeInMinutes = pump->WaitTimeInMinutes;
  auto waitTimeInSeconds = Converters::MinutesToSeconds(waitTimeInMinutes);
  auto workTimeInSeconds = pump->WorkTimeInSeconds;

  _autoPumpLcd.UpdateWaitTimeFromSeconds(waitTimeInSeconds);
  _autoPumpLcd.UpdateWorkTimeFromSeconds(workTimeInSeconds);
}

void UpdateSelectedPumpWaitAndWorkTime()
{
  auto pump = GetSelectedPump();
  auto workTimeInSeconds = _autoPumpLcd.ConvertWorkTimeToSeconds();
  auto waitTimeInSeconds = _autoPumpLcd.ConvertWaitTimeToSeconds();

  pump->WorkTimeInSeconds = workTimeInSeconds;
  pump->WaitTimeInMinutes = Converters::SecondsToMinutes(waitTimeInSeconds);
}

void TryPrintSelectedPumpStatus()
{
  auto currentState = _autoPumpStateMachine.GetState();
  switch (currentState)
  {
  case SelectPumpState:
  case SelectSettingsState:
    auto pump = GetSelectedPump();
    auto status = pump->GetStatus();
    _autoPumpLcd.PrintOnRow(1, status);
    break;
  }
}

#pragma region AutoPumpStateMachine Handlers
void OnStateChanged()
{
  _autoPumpLcd.UpdateStateIfNeeded(_autoPumpStateMachine.GetState());
  TryPrintSelectedPumpStatus();
}
void OnStateMachineLeftSettings()
{
  auto waitTimeInSeconds = _autoPumpLcd.ConvertWaitTimeToSeconds();
  auto workTimeInSeconds = _autoPumpLcd.ConvertWorkTimeToSeconds();
  UpdateSelectedValuesToSelectedPump(waitTimeInSeconds, workTimeInSeconds);
}
#pragma endregion

void setup()
{
  if(DEBUG)
    Serial.begin(9600);

  _autoPumpStateMachine.AttachOnIncreaseValue([]() { _autoPumpLcd.UpdateSelectedValues(1); });
  _autoPumpStateMachine.AttachOnDecreaseValue([]() { _autoPumpLcd.UpdateSelectedValues(-1); });
  _autoPumpStateMachine.AttachOnStateChanged(&OnStateChanged);
  _autoPumpStateMachine.AttachOnLeftSettings(&OnStateMachineLeftSettings);
  _autoPumpStateMachine.AttachOnBeforeEnterToSettings([]() { UpdateSelectedValuesFromSelectedPump(); });

  _pumpButton1.attachLongPressStart([]() { _pumps[0]->ForceStart(); });
  _pumpButton1.attachLongPressStop([]() { _pumps[0]->ForceStop(); });
  _pumpButton2.attachLongPressStart([]() { _pumps[1]->ForceStart(); });
  _pumpButton2.attachLongPressStop([]() { _pumps[1]->ForceStop(); });

  _pumps[0] = &_pump1;
  _pumps[1] = &_pump2;

  for (byte i = 0; i < PUPM_AMOUNT; i++)
  {
    auto pump = _pumps[i];
    pump->Init();
  }

  _autoPumpEncoder.SetEncoderType(ENCODER_TYPE);
  _autoPumpEncoder.SetEncoderDirection(IS_ENCODER_REVERSED);

  _autoPumpEncoder.AttachOnLeftTurn([]() { _autoPumpStateMachine.Run(EncoderLeftTurnCommand); });
  _autoPumpEncoder.AttachOnRightTurn([]() { _autoPumpStateMachine.Run(EncoderRightTurnCommand); });
  _autoPumpEncoder.AttachOnLeftHoldTurn([]() { _autoPumpStateMachine.Run(EncoderHoldLeftTurnCommand); });
  _autoPumpEncoder.AttachOnRightHoldTurn([]() { _autoPumpStateMachine.Run(EncoderHoldRightTurnCommand); });
  _autoPumpEncoder.AttachOnClick([]() { _autoPumpStateMachine.Run(EncoderClickCommand); });

  _autoPumpLcd.IsAutoOff = IS_LCD_AUTO_OFF;
  _autoPumpLcd.TimeoutInSeconds = Lcd_TIMEOUT_SECONDS;
  _autoPumpLcd.Init(PUPM_AMOUNT, _autoPumpStateMachine.GetState());
  _autoPumpLcd.AttachOnSelectedPumpChanged([]() { TryPrintSelectedPumpStatus(); });

  _timer.SetTimeout(1000);
  _timer.AttachOnTick([]() { TryPrintSelectedPumpStatus(); });
  _timer.Start();
}

void loop()
{
  _pumpButton1.tick();
  _pumpButton2.tick();
  _autoPumpEncoder.Tick();
  _autoPumpLcd.Tick();
  _timer.Tick();

  for (byte i = 0; i < PUPM_AMOUNT; i++)
  {
    auto pump = _pumps[i];
    if (_isWatering && IS_PARALLEL_WATERING_DISABLED && !pump->GetIsWorking())
        continue;

    pump->Tick();

    _isWatering = pump->GetIsWorking();
    if (_isWatering && IS_PARALLEL_WATERING_DISABLED)
      break;
  }
}
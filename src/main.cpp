#include "OneButton.h"
#include "AutoPumpEncoder/AutoPumpEncoder.h"
#include "AutoPumpLcd/AutoPumpLcd.h"
#include "AutoPumpStateMachine/AutoPumpStateMachine.h"
#include "Pump/Pump.h"
#include "settings.h"
#include "enums.h"
#include "converters.h"
#include "Timer/Timer.h"
#include "DataStorage/DataStorage.h"

AutoPumpStateMachine _autoPumpStateMachine;
AutoPumpEncoder _autoPumpEncoder = AutoPumpEncoder(PIN_EncoderClk, PIN_EncoderDt, PIN_EncoderSw);

OneButton _pumpButton1 = OneButton(PIN_Button1, true, true);
Pump _pump1 = Pump(PIN_Pump1);
OneButton _pumpButton2 = OneButton(PIN_Button2, true, true);
Pump _pump2 = Pump(PIN_Pump2);
AutoPumpLcd _autoPumpLcd = AutoPumpLcd(16, 2);
Timer _timer;

DataStorage _dataStorage(PUPM_AMOUNT);
Pump *_pumps[PUPM_AMOUNT]={ &_pump1, &_pump2};

bool _isWatering;

Pump* GetSelectedPump()
{
  auto pumpIndex = _autoPumpLcd.GetSelectedPumpIndex();
  auto pump = _pumps[pumpIndex];
  return pump;
}

void UpdateSelectedValuesToSelectedPump(unsigned long waitTimeInMinutes, unsigned long workTimeInSeconds)
{
  auto pumpIndex = _autoPumpLcd.GetSelectedPumpIndex();
  auto pump = _pumps[pumpIndex];

  pump->WaitTimeInMinutes = waitTimeInMinutes;
  pump->WorkTimeInSeconds = workTimeInSeconds;
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

Data CreateData(unsigned long waitTimeInMinutes, unsigned long workTimeInSeconds)
{
  Data data;
  data.WaitTimeInMinutes = waitTimeInMinutes;
  data.WorkTimeInSeconds = workTimeInSeconds;
  return data;
}
void SaveDataIfNeeded(int index, unsigned long waitTimeInMinutes, unsigned long workTimeInSeconds)
{
  auto data = CreateData(waitTimeInMinutes, workTimeInSeconds);
  _dataStorage.SaveDataIfNeeded(index, data);
}

#pragma region AutoPumpStateMachine Handlers
void OnStateChanged()
{
  _autoPumpLcd.UpdateStateIfNeeded(_autoPumpStateMachine.GetState());
  TryPrintSelectedPumpStatus();
}
void OnStateMachineLeftSettings()
{
  auto waitTimeInMinutes = Converters::SecondsToMinutes(_autoPumpLcd.ConvertWaitTimeToSeconds());
  auto workTimeInSeconds = _autoPumpLcd.ConvertWorkTimeToSeconds();
  UpdateSelectedValuesToSelectedPump(waitTimeInMinutes, workTimeInSeconds);
  SaveDataIfNeeded(_autoPumpLcd.GetSelectedPumpIndex(), waitTimeInMinutes, workTimeInSeconds);
}
#pragma endregion

void setup()
{
  if (DEBUG)
    Serial.begin(9600);

  _autoPumpLcd.IsAutoOff = IS_LCD_AUTO_OFF;
  _autoPumpLcd.TimeoutInSeconds = Lcd_TIMEOUT_SECONDS;
  _autoPumpLcd.Init(PUPM_AMOUNT, _autoPumpStateMachine.GetState());
  _autoPumpLcd.AttachOnSelectedPumpChanged([]() { TryPrintSelectedPumpStatus(); });

  _dataStorage.Init();

  for (int i = 0; i < PUPM_AMOUNT; i++)
  {
    auto pump = _pumps[i];
    pump->Init(FORCEDLY_STARTED_PUMP_SECONDS);
    auto isDataReady = _dataStorage.GetIsReady(i);
    if(isDataReady)
      {
        auto data = _dataStorage.GetData(i);
        pump->WaitTimeInMinutes = data->WaitTimeInMinutes; 
        pump->WorkTimeInSeconds = data->WorkTimeInSeconds; 
      }
    else
      _dataStorage.SaveDataIfNeeded(i, CreateData(pump->WaitTimeInMinutes, pump->WorkTimeInSeconds));
  }

  _autoPumpStateMachine.AttachOnIncreaseValue([]() { _autoPumpLcd.UpdateSelectedValues(1); });
  _autoPumpStateMachine.AttachOnDecreaseValue([]() { _autoPumpLcd.UpdateSelectedValues(-1); });
  _autoPumpStateMachine.AttachOnStateChanged(&OnStateChanged);
  _autoPumpStateMachine.AttachOnLeftSettings(&OnStateMachineLeftSettings);
  _autoPumpStateMachine.AttachOnBeforeEnterToSettings([]() { UpdateSelectedValuesFromSelectedPump(); });

  _pumpButton1.attachLongPressStart([]() { _pumps[0]->ForceStart(ForcedlyStarted); });
  _pumpButton1.attachLongPressStop([]() { _pumps[0]->Stop(); });
  _pumpButton1.attachDoubleClick([](){  _pumps[0]->ForceStart(ForcedlyStartedWithTimer); });
  _pumpButton2.attachLongPressStart([]() { _pumps[1]->ForceStart(ForcedlyStarted); });
  _pumpButton2.attachLongPressStop([]() { _pumps[1]->Stop(); });
  _pumpButton2.attachDoubleClick([](){  _pumps[1]->ForceStart(ForcedlyStartedWithTimer); });

  _autoPumpEncoder.SetEncoderType(ENCODER_TYPE);
  _autoPumpEncoder.SetEncoderDirection(IS_ENCODER_REVERSED);

  _autoPumpEncoder.AttachOnLeftTurn([]() { _autoPumpStateMachine.Run(EncoderLeftTurnCommand); });
  _autoPumpEncoder.AttachOnRightTurn([]() { _autoPumpStateMachine.Run(EncoderRightTurnCommand); });
  _autoPumpEncoder.AttachOnLeftHoldTurn([]() { _autoPumpStateMachine.Run(EncoderHoldLeftTurnCommand); });
  _autoPumpEncoder.AttachOnRightHoldTurn([]() { _autoPumpStateMachine.Run(EncoderHoldRightTurnCommand); });
  _autoPumpEncoder.AttachOnClick([]() { _autoPumpStateMachine.Run(EncoderClickCommand); });

  _timer.SetTimeout(1000);
  _timer.AttachOnTick([]() { TryPrintSelectedPumpStatus(); });
  _timer.Start();

  _autoPumpLcd.Refresh();
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
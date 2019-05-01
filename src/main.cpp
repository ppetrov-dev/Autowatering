#include "OneButton.h"
#include "AutoWateringEncoder/AutoWateringEncoder.h"
#include "AutoWateringLcd/AutoWateringLcd.h"
#include "AutoWateringStateMachine/AutoWateringStateMachine.h"
#include "Pump/Pump.h"
#include "settings.h"
#include "enums.h"
#include "converters.h"
#include "Timer/Timer.h"
#include "DataStorage/DataStorage.h"

AutoWateringStateMachine _autoWateringStateMachine;
AutoWateringEncoder _autoWateringEncoder = AutoWateringEncoder(PIN_EncoderClk, PIN_EncoderDt, PIN_EncoderSw);

OneButton _pumpButton1 = OneButton(PIN_Button1, true, true);
Pump _pump1 = Pump(PIN_Pump1);
OneButton _pumpButton2 = OneButton(PIN_Button2, true, true);
Pump _pump2 = Pump(PIN_Pump2);
AutoWateringLcd _autoWateringLcd = AutoWateringLcd(16, 2);
Timer _timer;

DataStorage _dataStorage(PUPM_AMOUNT);
Pump *_pumps[PUPM_AMOUNT]={ &_pump1, &_pump2};
OneButton* _buttons[PUPM_AMOUNT]={&_pumpButton1, &_pumpButton2};

bool _isWatering;

Pump* GetSelectedPump()
{
  auto pumpIndex = _autoWateringLcd.GetSelectedPumpIndex();
  auto pump = _pumps[pumpIndex];
  return pump;
}

void UpdateSelectedValuesToSelectedPump(unsigned long waitTimeInMinutes, unsigned long workTimeInSeconds)
{
  auto pumpIndex = _autoWateringLcd.GetSelectedPumpIndex();
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

  _autoWateringLcd.UpdateWaitTimeFromSeconds(waitTimeInSeconds);
  _autoWateringLcd.UpdateWorkTimeFromSeconds(workTimeInSeconds);
}

void UpdateSelectedPumpWaitAndWorkTime()
{
  auto pump = GetSelectedPump();
  auto workTimeInSeconds = _autoWateringLcd.ConvertWorkTimeToSeconds();
  auto waitTimeInSeconds = _autoWateringLcd.ConvertWaitTimeToSeconds();

  pump->WorkTimeInSeconds = workTimeInSeconds;
  pump->WaitTimeInMinutes = Converters::SecondsToMinutes(waitTimeInSeconds);
}

void TryPrintSelectedPumpStatus()
{
  auto currentState = _autoWateringStateMachine.GetState();
  switch (currentState)
  {
  case SelectPumpState:
  case SelectSettingsState:
    auto pump = GetSelectedPump();
    auto status = pump->GetStatus();
    _autoWateringLcd.PrintOnRow(1, status);
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

#pragma region AutoWateringStateMachine Handlers
void OnStateChanged()
{
  _autoWateringLcd.UpdateStateIfNeeded(_autoWateringStateMachine.GetState());
  TryPrintSelectedPumpStatus();
}
void OnStateMachineLeftSettings()
{
  auto waitTimeInMinutes = Converters::SecondsToMinutes(_autoWateringLcd.ConvertWaitTimeToSeconds());
  auto workTimeInSeconds = _autoWateringLcd.ConvertWorkTimeToSeconds();
  UpdateSelectedValuesToSelectedPump(waitTimeInMinutes, workTimeInSeconds);
  SaveDataIfNeeded(_autoWateringLcd.GetSelectedPumpIndex(), waitTimeInMinutes, workTimeInSeconds);
}
#pragma endregion

#pragma region Buttons Handlers
void OnButtonLongPressStart(int index){
  _pumps[index]->ForceStart(ForcedlyStarted); 
}
void OnButtonDoubleClick(int index){
  auto pump = _pumps[index];
  if(pump->GetIsWorking()){
    pump->Stop();
    return;
  }
  _pumps[index]->ForceStart(ForcedlyStartedWithTimer); 
}
void OnButtonLongPressStop(int index){
   _pumps[index]->Stop(); 
}
 
#pragma endregion
void setup()
{
  if (DEBUG)
    Serial.begin(9600);

  _autoWateringLcd.IsAutoOff = IS_LCD_AUTO_OFF;
  _autoWateringLcd.TimeoutInSeconds = Lcd_TIMEOUT_SECONDS;
  _autoWateringLcd.Init(PUPM_AMOUNT, _autoWateringStateMachine.GetState());
  _autoWateringLcd.AttachOnSelectedPumpChanged([]() { TryPrintSelectedPumpStatus(); });

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

  _autoWateringStateMachine.AttachOnIncreaseValue([]() { _autoWateringLcd.UpdateSelectedValues(1); });
  _autoWateringStateMachine.AttachOnDecreaseValue([]() { _autoWateringLcd.UpdateSelectedValues(-1); });
  _autoWateringStateMachine.AttachOnStateChanged(&OnStateChanged);
  _autoWateringStateMachine.AttachOnLeftSettings(&OnStateMachineLeftSettings);
  _autoWateringStateMachine.AttachOnBeforeEnterToSettings([]() { UpdateSelectedValuesFromSelectedPump(); });

  _pumpButton1.attachLongPressStart([]() { OnButtonLongPressStart(0); });
  _pumpButton1.attachLongPressStop([]() { OnButtonLongPressStop(0);});
  _pumpButton1.attachDoubleClick([](){ OnButtonDoubleClick(0);});
  _pumpButton2.attachLongPressStart([]() {OnButtonLongPressStart(1);});
  _pumpButton2.attachLongPressStop([]() { OnButtonLongPressStop(1); });
  _pumpButton2.attachDoubleClick([](){ OnButtonDoubleClick(1); });

  _autoWateringEncoder.SetEncoderType(ENCODER_TYPE);
  _autoWateringEncoder.SetEncoderDirection(IS_ENCODER_REVERSED);

  _autoWateringEncoder.AttachOnLeftTurn([]() { _autoWateringStateMachine.Run(EncoderLeftTurnCommand); });
  _autoWateringEncoder.AttachOnRightTurn([]() { _autoWateringStateMachine.Run(EncoderRightTurnCommand); });
  _autoWateringEncoder.AttachOnLeftHoldTurn([]() { _autoWateringStateMachine.Run(EncoderHoldLeftTurnCommand); });
  _autoWateringEncoder.AttachOnRightHoldTurn([]() { _autoWateringStateMachine.Run(EncoderHoldRightTurnCommand); });
  _autoWateringEncoder.AttachOnClick([]() { _autoWateringStateMachine.Run(EncoderClickCommand); });

  _timer.SetInterval(1000);
  _timer.AttachOnTick([]() { TryPrintSelectedPumpStatus(); });
  _timer.Start();

  _autoWateringLcd.Refresh();
}

void loop()
{
  _autoWateringEncoder.Tick();
  _autoWateringLcd.Tick();
  _timer.Tick();

  for (int i = 0; i < PUPM_AMOUNT; i++)
  {
    auto button = _buttons[i];
    button->tick();

    auto pump = _pumps[i];
    if (_isWatering && IS_PARALLEL_WATERING_DISABLED && !pump->GetIsWorking())
      continue;

    pump->Tick();

    _isWatering = pump->GetIsWorking();
    if (_isWatering && IS_PARALLEL_WATERING_DISABLED)
      break;
  }
}
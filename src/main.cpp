#include "OneButton.h"
#include "MyRotaryEncoder.h"
#include "MyTimer.h"
#include "AutoWateringLcd/AutoWateringLcd.h"
#include "AutoWateringUltrasonicSensor/AutoWateringUltrasonicSensor.h"
#include "AutoWateringStateMachine/AutoWateringStateMachine.h"
#include "Pump/Pump.h"
#include "settings.h"
#include "enums.h"
#include "MyDateTimeConverters.h"
#include "DataStorage/DataStorage.h"
#include "RTC/RealTimeClock.h"

AutoWateringUltrasonicSensor _autoWateringUltrasonicSensor;
AutoWateringStateMachine _autoWateringStateMachine;
MyRotaryEncoder _autoWateringEncoder = MyRotaryEncoder(PIN_EncoderClk, PIN_EncoderDt, PIN_EncoderSw);

AutoWateringLcd _autoWateringLcd = AutoWateringLcd(16, 2, &_autoWateringUltrasonicSensor);
MyTimer _oneSecondTimer;
MyTimer _twoSecondTimer;
RealTimeClock _realTimeClock;

DataStorage _dataStorage(PUMP_AMOUNT);
Pump *_pumps[PUMP_AMOUNT];

OneButton _pumpButton1 = OneButton(PIN_Button1, true, true);
OneButton _pumpButton2 = OneButton(PIN_Button2, true, true);
OneButton *_buttons[PUMP_AMOUNT] = {&_pumpButton1, &_pumpButton2};

bool _isWatering;

Pump *GetSelectedPump()
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
  auto waitTimeInSeconds = MyDateTimeConverters::MinutesToSeconds(waitTimeInMinutes);
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
  pump->WaitTimeInMinutes = MyDateTimeConverters::SecondsToMinutes(waitTimeInSeconds);
}

void TryUpdateSelectedPumpStatusOrInfo()
{
  switch (_autoWateringStateMachine.GetState())
  {
  case WatchInfoState:
    _autoWateringLcd.UpdateInfo();
    break;
  case SelectPumpState:
  case SelectSettingsState:
    auto pump = GetSelectedPump();
    auto status = pump->GetStatus();
    _autoWateringLcd.PrintOnRow(1, _autoWateringUltrasonicSensor.GetIsEmpty() ? "No water" : status);
    break;
  }
}
#pragma region Pump Handlers
void OnPumpStopped(Pump *pump)
{
  int pumpIndex = 0;
  while (_pumps[pumpIndex] != pump && pumpIndex <= PUMP_AMOUNT)
    pumpIndex++;
  if (pumpIndex == PUMP_AMOUNT)
    return;

  Serial.println("OnPumpStopped| pumpIndex: " + String(pumpIndex));
  _dataStorage.SaveDataIfNeeded(pumpIndex, _realTimeClock.GetNowTimeStamp());
}
#pragma endregion
#pragma region AutoWateringStateMachine Handlers
void OnStateChanged()
{
  auto currentState = _autoWateringStateMachine.GetState();
  _autoWateringLcd.UpdateState(currentState);
  TryUpdateSelectedPumpStatusOrInfo();
}
void OnStateMachineLeftSettings()
{
  auto waitTimeInSeconds = _autoWateringLcd.ConvertWaitTimeToSeconds();
  auto waitTimeInMinutes = MyDateTimeConverters::SecondsToMinutes(waitTimeInSeconds);
  auto workTimeInSeconds = _autoWateringLcd.ConvertWorkTimeToSeconds();
  UpdateSelectedValuesToSelectedPump(waitTimeInMinutes, workTimeInSeconds);
  _dataStorage.SaveDataIfNeeded(_autoWateringLcd.GetSelectedPumpIndex(), waitTimeInMinutes, workTimeInSeconds);
}
#pragma endregion

#pragma region Buttons Handlers
void OnButtonLongPressStart(int index)
{
  _pumps[index]->ForceStart(ForcedlyStarted);
}
void OnButtonDoubleClick(int index)
{
  auto pump = _pumps[index];
  if (pump->GetIsWorking())
  {
    pump->Stop();
    return;
  }
  _pumps[index]->ForceStart(ForcedlyStartedWithTimer);
}
void OnButtonLongPressStop(int index)
{
  _pumps[index]->Stop();
}

#pragma endregion
void setup()
{
  if (DEBUG)
  {
    Serial.begin(9600);
    Serial.println("Serial OK");
  }
  Wire.begin();
  _realTimeClock.Begin();
  Serial.print("Current DateTime: ");
  Serial.println(_realTimeClock.GetStringNow());

  _autoWateringLcd.IsAutoOff = IS_LCD_AUTO_OFF;
  _autoWateringLcd.TimeoutInSeconds = LCD_TIMEOUT_SECONDS;
  _autoWateringLcd.Init(PUMP_AMOUNT);
  _autoWateringLcd.AttachOnSelectedPumpChanged([]() { TryUpdateSelectedPumpStatusOrInfo(); });
  _autoWateringLcd.Refresh(_autoWateringStateMachine.GetState());

  _dataStorage.Init();
  auto nowTimeStampInSeconds = _realTimeClock.GetNowTimeStamp();
  for (int i = 0; i < PUMP_AMOUNT; i++)
  {
    auto pump = new Pump(PIN_FirstPump + i);
    _pumps[i] = pump;
    pump->Init(FORCEDLY_STARTED_PUMP_SECONDS, RELAY_TYPE);
    auto isDataStorageReady = _dataStorage.GetIsReady(i);
    if (isDataStorageReady)
    {
      auto data = _dataStorage.GetData(i);
      pump->WaitTimeInMinutes = data->WaitTimeInMinutes;
      pump->WorkTimeInSeconds = data->WorkTimeInSeconds;
      auto timeStampInSeconds = data->LastWateringTimeStampInSeconds;

      auto timeOffsetInSeconds = nowTimeStampInSeconds - timeStampInSeconds;
      if (timeOffsetInSeconds >= ACCEPTABLE_TIME_OFFSET_SECONDS)
        pump->ResetOffsetTime(timeOffsetInSeconds);
    }
    else
      _dataStorage.SaveDataIfNeeded(i, pump->WaitTimeInMinutes, pump->WorkTimeInSeconds, nowTimeStampInSeconds);
    pump->AttachOnStopped(&OnPumpStopped);
  }

  _autoWateringStateMachine.AttachOnIncreaseValue([]() { _autoWateringLcd.UpdateSelectedValues(_autoWateringStateMachine.GetState(), 1); });
  _autoWateringStateMachine.AttachOnDecreaseValue([]() { _autoWateringLcd.UpdateSelectedValues(_autoWateringStateMachine.GetState(), -1); });
  _autoWateringStateMachine.AttachOnStateChanged(&OnStateChanged);
  _autoWateringStateMachine.AttachOnLeftSettings(&OnStateMachineLeftSettings);
  _autoWateringStateMachine.AttachOnBeforeEnterToSettings([]() { UpdateSelectedValuesFromSelectedPump(); });

  pinMode(PIN_Button1, INPUT_PULLUP);
  pinMode(PIN_Button2, INPUT_PULLUP);
  _pumpButton1.attachLongPressStart([]() { 
    Serial.println("Button Long Press Start: 0");
    OnButtonLongPressStart(0); });
  _pumpButton1.attachLongPressStop([]() { 
    Serial.println("Button Long Press Stop: 0");
    OnButtonLongPressStop(0); });
  _pumpButton1.attachDoubleClick([]() { 
    Serial.println("Button Double Click: 0");
    OnButtonDoubleClick(0); });
  _pumpButton2.attachLongPressStop([]() { 
    Serial.println("Button Long Press Stop: 1");
    OnButtonLongPressStop(1); });
  _pumpButton2.attachLongPressStart([]() {
    Serial.println("Button Long Press Start: 1");
     OnButtonLongPressStart(1); });
  _pumpButton2.attachDoubleClick([]() { 
    Serial.println("Button Double Click: 1");
    OnButtonDoubleClick(1); });

  _autoWateringEncoder.SetEncoderType(ENCODER_TYPE);
  _autoWateringEncoder.SetEncoderDirection(IS_ENCODER_REVERSED);

  _autoWateringEncoder.AttachOnLeftTurn([]() { 
    Serial.println("Encoder Left Turn");
    _autoWateringStateMachine.Run(EncoderLeftTurnCommand); });
  _autoWateringEncoder.AttachOnRightTurn([]() { 
    Serial.println("Encoder Right Turn");
    _autoWateringStateMachine.Run(EncoderRightTurnCommand); });
  _autoWateringEncoder.AttachOnLeftHoldTurn([]() { 
    Serial.println("Encoder Left Hold Turn");
    _autoWateringStateMachine.Run(EncoderHoldLeftTurnCommand); });
  _autoWateringEncoder.AttachOnRightHoldTurn([]() { 
    Serial.println("Encoder Right Hold Turn");
    _autoWateringStateMachine.Run(EncoderHoldRightTurnCommand); });
  _autoWateringEncoder.AttachOnClick([]() { 
    Serial.println("Encoder Click");
    _autoWateringStateMachine.Run(EncoderClickCommand); });

  _oneSecondTimer.SetInterval(1000);
  _oneSecondTimer.AttachOnTick([]() { TryUpdateSelectedPumpStatusOrInfo(); });
  _oneSecondTimer.Start();

  _twoSecondTimer.SetInterval(2000);
  _twoSecondTimer.AttachOnTick([]() { _autoWateringUltrasonicSensor.Tick(); });
  _twoSecondTimer.Start();
}

void HandleButtonsTick()
{
  for (int i = 0; i < PUMP_AMOUNT; i++)
  {
    auto button = _buttons[i];
    button->tick();
  }
}

void UpdateIsWatering()
{
  for (int i = 0; i < PUMP_AMOUNT; i++)
  {
    auto pump = _pumps[i];
    _isWatering = pump->GetIsWorking();
    if (_isWatering)
      break;
  }
}

void HandlePumpsTick()
{
  for (int i = 0; i < PUMP_AMOUNT; i++)
  {
    auto pump = _pumps[i];
    auto beforeIsPumpWatering = pump->GetIsWorking();
    if (_isWatering && IS_PARALLEL_WATERING_DISABLED && !beforeIsPumpWatering)
      continue;

    pump->Tick();

    if (pump->GetIsWorking() && IS_PARALLEL_WATERING_DISABLED)
      break;
  }
}
void loop()
{
  _autoWateringEncoder.Tick();
  _autoWateringLcd.Tick();
  _oneSecondTimer.Tick();
  _twoSecondTimer.Tick();
  HandleButtonsTick();

  if (_autoWateringUltrasonicSensor.GetIsEmpty())
  {
    for (int i = 0; i < PUMP_AMOUNT; i++)
      if (_pumps[i]->GetIsWorking())
        _pumps[i]->Stop();

    return;
  }

  UpdateIsWatering();
  HandlePumpsTick();
}
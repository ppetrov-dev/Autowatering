#include <EEPROMex.h>
#include <EEPROMVar.h>
#include "OneButton.h"

#include "AutoPumpEncoder.h"
#include "AutoPumpLcd.h"
#include "AutoPumpStateMachine.h"
#include "Pump.h"
#include "settings.h"
#include "enums.h"
#include "converters.h"
#include "Timer.h"

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
  // EEPROM.updateLong(8 * _selectedPumpNumber, _pumpPauseTimesInMinutes[_selectedPumpNumber]);
  // EEPROM.updateLong(8 * _selectedPumpNumber + 4, _pumpWorkTimesInMinutes[_selectedPumpNumber]);
}

void UpdateSelectedValuesToSelectedPump()
{
  auto pumpIndex = _autoPumpLcd.GetSelectedPumpIndex();
  auto pump = _pumps[pumpIndex];

  auto pauseTimeInSeconds = _autoPumpLcd.ConvertPauseTimeToSeconds();
  auto workTimeInSeconds = _autoPumpLcd.ConvertWorkTimeToSeconds();

  pump->PauseTimeInMinutes = Converters::SecondsToMinutes(pauseTimeInSeconds);
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
  auto pauseTimeInMinutes = pump->PauseTimeInMinutes;
  auto pauseTimeInSeconds = Converters::MinutesToSeconds(pauseTimeInMinutes);
  auto workTimeInSeconds = pump->WorkTimeInSeconds;

  _autoPumpLcd.UpdatePauseTimeFromSeconds(pauseTimeInSeconds);
  _autoPumpLcd.UpdateWorkTimeFromSeconds(workTimeInSeconds);
}

void SwitchPumpOn(byte pumpIndex)
{
  _isWatering = true;
  auto pump = _pumps[pumpIndex];
  pump->SwitchOn();
  Serial.println("Pump #" + String(pumpIndex + 1) + " ON");
}

void SwitchPumpOff(byte pumpIndex)
{
  auto pump = _pumps[pumpIndex];
  pump->SwitchOff();
  Serial.println("Pump #" + String(pumpIndex + 1) + " OFF");
  _isWatering = false;
}

void CheckPumpsToWaterAndSwitchOnIfNeeded()
{
  if (_isWatering)
    return;

  for (byte pumpIndex = 0; pumpIndex < PUPM_AMOUNT; pumpIndex++)
  {
    auto pump = _pumps[pumpIndex];
    auto isTimeForWatering = pump->GetIsTimeForWatering();

    if (isTimeForWatering)
    {
      SwitchPumpOn(pumpIndex);
      break;
    }
  }
}

void CheckPumpsToWaterAndSwitchOffIfNeeded()
{
  if (!_isWatering)
    return;

  for (byte pumpIndex = 0; pumpIndex < PUPM_AMOUNT; pumpIndex++)
  {
    auto pump = _pumps[pumpIndex];
    auto isTimeToStopWatering = pump->GetIsTimeToStopWatering();

    if (!isTimeToStopWatering)
      continue;

    SwitchPumpOff(pumpIndex);
  }
}

void UpdateSelectedPumpPauseAndWorkTime()
{
  auto pump = GetSelectedPump();
  auto workTimeInSeconds = _autoPumpLcd.ConvertWorkTimeToSeconds();
  auto pauseTimeInSeconds = _autoPumpLcd.ConvertPauseTimeToSeconds();

  pump->WorkTimeInSeconds = workTimeInSeconds;
  pump->PauseTimeInMinutes = Converters::SecondsToMinutes(pauseTimeInSeconds);
}

void TryForceStartWatering(byte pumpIndex)
{
  if (_isWatering)
  {
    Serial.println("Force Watering could not be started for Pump #" + String(pumpIndex + 1));
    Serial.println("Only one pump could be enabled at the same time");
    return;
  }
  SwitchPumpOn(pumpIndex);
}

void TryForceStopWatering(byte pumpIndex)
{
  if (!_isWatering)
  {
    Serial.println("Force Watering could not be stopped for Pump #" + String(pumpIndex + 1));
    Serial.println("There are no enabled pumps");
    return;
  }
  SwitchPumpOff(pumpIndex);
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
    _autoPumpLcd.PrintToRow(1, status);
    break;
  default:
    break;
  }
}
#pragma region AutoPumpLcd Handlers
void OnSelectedPumpChanged()
{
  TryPrintSelectedPumpStatus();
}
#pragma endregion

#pragma region AutoPumpStateMachine Handlers
void OnStateChanged()
{
  _autoPumpLcd.UpdateStateIfNeeded(_autoPumpStateMachine.GetState());
  TryPrintSelectedPumpStatus();
}
void OnStateMachineLeftSettings()
{
  //save data if needed
  UpdateSelectedValuesToSelectedPump();
}
void OnBeforeEnterToSettings()
{
  UpdateSelectedValuesFromSelectedPump();
}
#pragma endregion

void setup()
{
  Serial.begin(9600);

  pinMode(PIN_Button1, INPUT_PULLUP);
  pinMode(PIN_Button2, INPUT_PULLUP);

  _autoPumpStateMachine.AttachOnIncreaseValue([]() { _autoPumpLcd.UpdateSelectedValues(1); });
  _autoPumpStateMachine.AttachOnDecreaseValue([]() { _autoPumpLcd.UpdateSelectedValues(-1); });
  _autoPumpStateMachine.AttachOnStateChanged(&OnStateChanged);
  _autoPumpStateMachine.AttachOnLeftSettings(&OnStateMachineLeftSettings);
  _autoPumpStateMachine.AttachOnBeforeEnterToSettings(&OnBeforeEnterToSettings);

  _pumpButton1.attachLongPressStart([]() { TryForceStartWatering(0); });
  _pumpButton1.attachLongPressStop([]() { TryForceStopWatering(0); });
  _pumpButton2.attachLongPressStart([]() { TryForceStartWatering(1); });
  _pumpButton2.attachLongPressStop([]() { TryForceStopWatering(1); });

  _pumps[0] = &_pump1;
  _pumps[1] = &_pump2;

  for (byte i = 0; i < PUPM_AMOUNT; i++)
  {
    auto pump = _pumps[i];
    pump->Init();
    pump->SetStartTimerOption(START_TIMER_OPTION);
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
  _autoPumpLcd.AttachOnSelectedPumpChanged(&OnSelectedPumpChanged);

  _timer.SetTimeout(1000);
  _timer.AttachOnTick([]() { TryPrintSelectedPumpStatus(); });
  _timer.Start();

  // _autoPumpEncoder.Tick();
  //   if (_autoPumpEncoder.IsHold())
  //   {
  //     _lcd.setCursor(0, 0);
  //     _lcd.print("Reset settings...");
  //     //  EEPROM.clear();
  //   }
  //   while (!_autoPumpEncoder.IsHold())
  //     ;
  //   _lcd.clear(); // очищаем дисплей, продолжаем работу

  // // в ячейке 1023 должен быть записан флажок, если его нет - делаем (ПЕРВЫЙ ЗАПУСК)
  // if (EEPROM.read(1023) != 5)
  // {
  //   EEPROM.writeByte(1023, 5);

  //   // для порядку сделаем 1 ячейки с 0 по 500
  //   for (byte i = 0; i < 500; i += 4)
  //   {
  //     EEPROM.writeLong(i, 0);
  //   }
  // }

  // for (byte i = 0; i < PUPM_AMOUNT; i++)
  // {
  //   _pumpPauseTimesInMinutes[i] = EEPROM.readLong(8 * i);    // читаем данные из памяти. На чётных - период (ч)
  //   _pumpWorkTimesInMinutes[i] = EEPROM.readLong(8 * i + 4); // на нечётных - полив (с)
  //   _pumpStates[i] = DEFAULT_PUMP_STATE;
  // }
}

void loop()
{
  _pumpButton1.tick();
  _pumpButton2.tick();
  _autoPumpEncoder.Tick();
  _autoPumpLcd.Tick();
  _timer.Tick();

  CheckPumpsToWaterAndSwitchOnIfNeeded();
  CheckPumpsToWaterAndSwitchOffIfNeeded();
}
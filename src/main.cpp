#ifndef arduinoH
#define arduinoH
#include <Arduino.h>
#endif
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

AutoPumpStateMachine _autoPumpStateMachine;
AutoPumpEncoder _autoPumpEncoder = AutoPumpEncoder(PIN_EncoderClk, PIN_EncoderDt, PIN_EncoderSw);
OneButton _pumpButton1 = OneButton(PIN_Button1, true);
Pump _pump1 = Pump(PIN_Pump1);
OneButton _pumpButton2 = OneButton(PIN_Button2, true);
Pump _pump2 = Pump(PIN_Pump2);
AutoPumpLcd _autoPumpLcd = AutoPumpLcd(16, 2, PUPM_AMOUNT);

Pump *_pumps[PUPM_AMOUNT];

bool _isWatering;

void UpdateDataInMemoryForSelectedPump()
{
  // EEPROM.updateLong(8 * _selectedPumpNumber, _pumpPauseTimesInMinutes[_selectedPumpNumber]);
  // EEPROM.updateLong(8 * _selectedPumpNumber + 4, _pumpWorkTimesInMinutes[_selectedPumpNumber]);
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

void CheckPumpsToWaterAndOnIfNeeded()
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

void CheckPumpsToWaterAndOffIfNeeded()
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
  auto pumpIndex = _autoPumpLcd.GetSelectedPumpIndex();
  auto pump = _pumps[pumpIndex];
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

#pragma region PumpButtons Handers

void OnLongPressButton1Start()
{
  TryForceStartWatering(0);
}
void OnLongPressButton1Stop()
{
  TryForceStopWatering(0);
}
void OnLongPressButton2Start()
{
  TryForceStartWatering(1);
}
void OnLongPressButton2Stop()
{
  TryForceStopWatering(1);
}

#pragma endregion

#pragma region Encoder Handlers
void OnAutoPumpEncoderLeftTurn()
{
  _autoPumpStateMachine.Run(EncoderLeftTurnCommand);
  //  _autoPumpLcd.MoveToPreviousCursorPosition();
  //   OnAutoPumpLcdCursorPositionChanged();
}

void OnAutoPumpEncoderRightTurn()
{
  _autoPumpStateMachine.Run(EncoderRightTurnCommand);

  // _autoPumpLcd.MoveToNextCursorPosition();
  // OnAutoPumpLcdCursorPositionChanged();
}
void OnAutoPumpEncoderClick()
{
  _autoPumpStateMachine.Run(EncoderClickCommand);

  // auto cursorPosition = _autoPumpLcd.GetCursorPosition();

  // switch (cursorPosition)
  // {
  // case SelectSettings:
  //   _autoPumpLcd.GoToSettings();
  //   break;
  // case SelectBack:
  //   _autoPumpLcd.LeaveSettings();
  //   break;
  // default:
  //   break;
  // }
}
void OnAutoPumpEncoderLeftHoldTurn()
{
  _autoPumpStateMachine.Run(EncoderHoldLeftTurnCommand);
}

void OnAutoPumpEncoderRightHoldTurn()
{
  _autoPumpStateMachine.Run(EncoderHoldRightTurnCommand);
}

#pragma endregion

#pragma region AutoPumpStateMachine Handlers

void OnStateMachineIncreaseValue()
{
  //  _autoPumpLcd.UpdateSelectedValues(1);
}
void OnStateMachineDecreaseValue()
{
  // _autoPumpLcd.UpdateSelectedValues(-1);
}
void OnStateMachineStateChanged()
{

}
void OnStateMachineLeftSettings(){
  
}
#pragma endregion

void setup()
{
  Serial.begin(9600);

  pinMode(PIN_Button1, INPUT_PULLUP);
  pinMode(PIN_Button2, INPUT_PULLUP);

  _autoPumpStateMachine.AttachOnIncreaseValue(&OnStateMachineIncreaseValue);
  _autoPumpStateMachine.AttachOnDecreaseValue(&OnStateMachineDecreaseValue);
  _autoPumpStateMachine.AttachOnStateChanged(&OnStateMachineStateChanged);
  _autoPumpStateMachine.AttachOnLeftSettings(&OnStateMachineLeftSettings);

  _pumpButton1.attachLongPressStart(&OnLongPressButton1Start);
  _pumpButton1.attachLongPressStop(&OnLongPressButton1Stop);
  _pumpButton2.attachLongPressStart(&OnLongPressButton2Start);
  _pumpButton2.attachLongPressStop(&OnLongPressButton2Stop);

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

  _autoPumpEncoder.AttachOnLeftTurn(&OnAutoPumpEncoderLeftTurn);
  _autoPumpEncoder.AttachOnRightTurn(&OnAutoPumpEncoderRightTurn);
  _autoPumpEncoder.AttachOnLeftHoldTurn(&OnAutoPumpEncoderLeftHoldTurn);
  _autoPumpEncoder.AttachOnRightHoldTurn(&OnAutoPumpEncoderRightHoldTurn);
  _autoPumpEncoder.AttachOnClick(&OnAutoPumpEncoderClick);

  _autoPumpLcd.Init();
  _autoPumpLcd.IsAutoOff = IS_LCD_AUTO_OFF;
  _autoPumpLcd.TimeoutInSeconds = Lcd_TIMEOUT_SECONDS;

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

  _autoPumpLcd.Refresh();
}

void loop()
{
  _pumpButton1.tick();
  _pumpButton2.tick();
  _autoPumpEncoder.Tick();
  _autoPumpLcd.Tick();

  CheckPumpsToWaterAndOnIfNeeded();
  CheckPumpsToWaterAndOffIfNeeded();
}
#include <Arduino.h>
#include <EEPROMex.h>
#include <EEPROMVar.h>
#include "OneButton.h"

#include "AutoPumpEncoder.h"
#include "AutoPumpLcd.h"
#include "Pump.h"
#include "settings.h"
#include "enums.h"

AutoPumpEncoder _autoPumpEncoder(PIN_encoderClk, PIN_encoderDt, PIN_encoderSw);
OneButton _pumpButton1(PIN_Button1, true);
Pump _pump1(PIN_Pump1);
OneButton _pumpButton2(PIN_Button2, true);
Pump _pump2(PIN_Pump2);
AutoPumpLcd _autoPumpLcd(16, 2);

Pump _pumps[]{
    _pump1,
    _pump2};

bool _isWatering;

unsigned long _lastEncoderActivityTimeInMilliseconds;

void UpdateDataInMemoryForSelectedPump()
{
  // EEPROM.updateLong(8 * _selectedPumpNumber, _pumpPauseTimesInMinutes[_selectedPumpNumber]);
  // EEPROM.updateLong(8 * _selectedPumpNumber + 4, _pumpWorkTimesInMinutes[_selectedPumpNumber]);
}

bool IsLcdTimeoutExpired()
{
  auto _lcdTimeoutInSeconds = _autoPumpLcd.GetTimeoutInSeconds();
  auto lcdTimeoutInMilliseconds = ConvertSecondsToMilliseconds(_lcdTimeoutInSeconds);
  return millis() - _lastEncoderActivityTimeInMilliseconds >= lcdTimeoutInMilliseconds;
}

void SwitchLcdOffIfNeeded()
{
  if (!_autoPumpLcd.GetIsAutoOff() || !_autoPumpLcd.GetIsOn() || !IsLcdTimeoutExpired())
    return;

  _autoPumpLcd.SwitchOff();
}

void SwitchPumpOn(byte pumpIndex)
{
  _isWatering = true;
  auto pump = _pumps[pumpIndex];
  pump.SwitchOn();
  Serial.println("Pump #" + String(pumpIndex + 1) + " ON");
}

void SwitchPumpOff(byte pumpIndex)
{
  auto pump = _pumps[pumpIndex];
  pump.SwitchOff();
   Serial.println("Pump #" + String(pumpIndex + 1) + " OFF");
  _isWatering = false;
}

void CheckPumpsToWaterAndOnIfNeeded()
{
  if(_isWatering)
    return;

  for (byte pumpIndex = 0; pumpIndex < PUPM_AMOUNT; pumpIndex++)
  {
    auto pump = _pumps[pumpIndex];
    auto isTimeForWatering = pump.GetIsTimeForWatering();

    if(!isTimeForWatering)
      continue;

    SwitchPumpOn(pumpIndex);
  }
}

void CheckPumpsToWaterAndOffIfNeeded()
{
  if (!_isWatering)
    return;

for (byte pumpIndex = 0; pumpIndex < PUPM_AMOUNT; pumpIndex++)
  {
    auto pump = _pumps[pumpIndex];
    auto isTimeToStopWatering = pump.GetIsTimeToStopWatering();

    if(!isTimeToStopWatering)
      continue;

    SwitchPumpOff(pumpIndex);
  }
}

void UpdatePauseOrWorkTimeToSaveInMemory()
{
  // uint32_t newTime = ConvertSelectedDaysHoursMinutesToMinutes();
  // bool isNewPauseTime = _lcdCursorPosition < SelectWorkDays;

  // if (isNewPauseTime)
  //   _pumpPauseTimesInMinutes[_selectedPumpNumber] = newTime;
  // else
  //   _pumpWorkTimesInMinutes[_selectedPumpNumber] = newTime;
}

void TryForceStartWatering(byte pumpIndex){
  if(_isWatering)
    {
      Serial.println("Force Watering could not be started for Pump #" + String(pumpIndex + 1));
      Serial.println("Only one pump could be enabled at the same time");
      return;
    }
  SwitchPumpOn(0);
}

void TryForceStopWatering(byte pumpIndex){
  if(!_isWatering)
    {
      Serial.println("Force Watering could not be stopped for Pump #" + String(pumpIndex + 1));
      Serial.println("There are no enabled pumps");
      return;
    }
  SwitchPumpOff(0);
}

void OnLongPressButton1Start()
{
  Serial.println("OnLongPressButton1Start");
  TryForceStartWatering(0);
}
void OnLongPressButton1Stop()
{
  Serial.println("OnLongPressButton1Stop");
   TryForceStopWatering(0);
}
void OnLongPressButton2Start()
{
  Serial.println("OnLongPressButton2Start");
  TryForceStartWatering(1);
}
void OnLongPressButton2Stop()
{
  Serial.println("OnLongPressButton2Stop");
   TryForceStopWatering(1);
}

void OnAutoPumpEncoderAnyTurn()
{
  Serial.println("OnAutoPumpEncoderAnyTurn");

  _lastEncoderActivityTimeInMilliseconds = millis();

  if (_autoPumpLcd.GetIsOn())
    return;

  _autoPumpLcd.SwitchOn();
}

void ExcecuteIfLcdIsOn(void (*function)(void))
{
  if (!_autoPumpLcd.GetIsOn())
    return;

  function();

  _autoPumpLcd.PrintDataAndUpdateArrowPosition();
}

void OnAutoPumpLcdCursorPositionChanged()
{
  if (_autoPumpLcd.GetIsWatchingPauseStates())
  {
    //_autoPumpLcd.UpdateSelectedValuesFromMinutes(_pumpPauseTimesInMinutes[_selectedPumpIndex]);
  }
  else
  {
    //_autoPumpLcd.UpdateSelectedValuesFromMinutes(_pumpWorkTimesInMinutes[_selectedPumpIndex]);
  }
}

void OnAutoPumpEncoderLeftTurn()
{
  Serial.println("OnAutoPumpEncoderLeftTurn");
  ExcecuteIfLcdIsOn([]() {
    _autoPumpLcd.MoveToPreviousCursorPosition();
    OnAutoPumpLcdCursorPositionChanged();
  });
}

void OnAutoPumpEncoderRightTurn()
{
  Serial.println("OnAutoPumpEncoderRightTurn");
  ExcecuteIfLcdIsOn([]() {
    _autoPumpLcd.MoveToNextCursorPosition();
    OnAutoPumpLcdCursorPositionChanged();
  });
}

void OnAutoPumpLcdSelectedValuesChangedWhenHold()
{
  if (_autoPumpLcd.GetCursorPosition() == SelectPump)
  {
    _autoPumpLcd.PrintSelectedPumpName();
    // uint32_t selectedPumpSavedMinutes = _pumpPauseTimesInMinutes[_selectedPumpIndex];
    // _autoPumpLcd.UpdateSelectedValuesFromMinutes(selectedPumpSavedMinutes);
  }
  else
    UpdatePauseOrWorkTimeToSaveInMemory();
}

void OnAutoPumpEncoderLeftHoldTurn()
{
  Serial.println("OnAutoPumpEncoderLeftHoldTurn");
  ExcecuteIfLcdIsOn([]() {
    _autoPumpLcd.UpdateSelectedValues(-1);
    OnAutoPumpLcdSelectedValuesChangedWhenHold();
  });
}

void OnAutoPumpEncoderRightHoldTurn()
{
  Serial.println("OnAutoPumpEncoderRightHoldTurn");
  ExcecuteIfLcdIsOn([]() {
    _autoPumpLcd.UpdateSelectedValues(1);
    OnAutoPumpLcdSelectedValuesChangedWhenHold();
  });
}

void setup()
{
  Serial.begin(9600);

  pinMode(PIN_Button1, INPUT_PULLUP);
  pinMode(PIN_Button2, INPUT_PULLUP);

  _pumpButton1.attachLongPressStart(OnLongPressButton1Start);
  _pumpButton1.attachLongPressStop(OnLongPressButton1Stop);
  _pumpButton2.attachLongPressStart(OnLongPressButton2Start);
  _pumpButton2.attachLongPressStop(OnLongPressButton2Stop);
  
  for (byte i = 0; i < PUPM_AMOUNT; i++)
  {
    auto pump = _pumps[i];
    pump.Init();
    pump.SetStartTimerOption(START_TIMER_OPTION);
  }

  _autoPumpEncoder.SetEncoderType(ENCODER_TYPE);
  _autoPumpEncoder.SetEncoderDirection(IS_ENCODER_REVERSED);

  _autoPumpEncoder.AttachOnAnyTurn(OnAutoPumpEncoderAnyTurn);
  _autoPumpEncoder.AttachOnLeftTurn(OnAutoPumpEncoderLeftTurn);
  _autoPumpEncoder.AttachOnRightTurn(OnAutoPumpEncoderRightTurn);
  _autoPumpEncoder.AttachOnLeftHoldTurn(OnAutoPumpEncoderLeftHoldTurn);
  _autoPumpEncoder.AttachOnRightHoldTurn(OnAutoPumpEncoderRightHoldTurn);

  _autoPumpLcd.Init();
  _autoPumpLcd.SwitchOn();

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

  CheckPumpsToWaterAndOnIfNeeded();
  CheckPumpsToWaterAndOffIfNeeded();
  SwitchLcdOffIfNeeded();
}
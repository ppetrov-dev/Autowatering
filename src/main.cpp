#include <Arduino.h>
#include <EEPROMex.h>
#include <EEPROMVar.h>
#include "OneButton.h"

#include "AutoPumpEncoder.h"
#include "AutoPumpLcd.h"
#include "converters.h"
#include "settings.h"
#include "enums.h"

AutoPumpEncoder _autoPumpEncoder(PIN_clk, PIN_dt, PIN_sw);
OneButton _pumpButton1(PIN_Button1, true);
OneButton _pumpButton2(PIN_Button2, true);
AutoPumpLcd _autoPumpLcd(16, 2);

uint32_t _pumpTimersInMilliseconds[PUPM_AMOUNT];
uint32_t _pumpWorkTimesInMinutes[PUPM_AMOUNT];
uint32_t _pumpPauseTimesInMinutes[PUPM_AMOUNT];
bool _pumpStates[PUPM_AMOUNT];
byte _pumpPins[PUPM_AMOUNT];

bool _isPumping;

bool startFlag = true;

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

void TurnPumpOn(byte pumpNumber)
{
  digitalWrite(_pumpPins[pumpNumber], !DEFAULT_PUMP_STATE);
  _isPumping = true;

  if (START_TIMER_OPTION == WhenPumpIsOn)
    _pumpTimersInMilliseconds[pumpNumber] = millis();

  //Serial.println("Pump #" + String(i) + " ON");
}

void TurnPumpOff(byte pumpNumber)
{
  digitalWrite(_pumpPins[pumpNumber], DEFAULT_PUMP_STATE);
  _isPumping = false;

  if (START_TIMER_OPTION == WhenPumpIsOff)
    _pumpTimersInMilliseconds[pumpNumber] = millis();

  //Serial.println("Pump #" + String(i) + " OFF");
}

void CheckPumpsToWaterAndOnIfNeeded()
{
  if (startFlag)
  {
    startFlag = false;
    return;
  }

  auto currentMilliseconds = millis();
  for (byte pumpNumber = 0; pumpNumber < PUPM_AMOUNT; pumpNumber++)
  {
    if (_pumpPauseTimesInMinutes[pumpNumber] == 0 || _isPumping)
      continue;

    auto pumpPauseTimeInMilliseconds = ConvertMinutesToMilliseconds(_pumpPauseTimesInMinutes[pumpNumber]);
    auto passedTimeinMilliseconds = currentMilliseconds - _pumpTimersInMilliseconds[pumpNumber];
    if (passedTimeinMilliseconds < pumpPauseTimeInMilliseconds)
      continue;

    TurnPumpOn(pumpNumber);
  }
}

void CheckPumpsToWaterAndOffIfNeeded()
{
  if (!_isPumping)
    return;

  auto currentMilliseconds = millis();
  for (byte pumpNumber = 0; pumpNumber < PUPM_AMOUNT; pumpNumber++)
  {
    if (_pumpWorkTimesInMinutes[pumpNumber] == 0 || _pumpStates[pumpNumber] != DEFAULT_PUMP_STATE)
      continue;

    auto pumpWorkTimeInMilliseconds = ConvertMinutesToMilliseconds(_pumpWorkTimesInMinutes[pumpNumber]);
    auto passedTimeinMilliseconds = currentMilliseconds - _pumpTimersInMilliseconds[pumpNumber];
    if (passedTimeinMilliseconds < pumpWorkTimeInMilliseconds)
      continue;

    TurnPumpOff(pumpNumber);
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

void ForceWatering(unsigned int pin, bool isOn)
{
  digitalWrite(pin, isOn);
}

void OnLongPressButton1Start()
{
  Serial.println("OnLongPressButton1Start");
  ForceWatering(_pumpPins[0], HIGH);
}
void OnLongPressButton1Stop()
{
  Serial.println("OnLongPressButton1Stop");
  ForceWatering(_pumpPins[0], LOW);
}
void OnLongPressButton2Start()
{
  Serial.println("OnLongPressButton2Start");
  ForceWatering(_pumpPins[1], HIGH);
}
void OnLongPressButton2Stop()
{
  Serial.println("OnLongPressButton2Stop");
  ForceWatering(_pumpPins[1], LOW);
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
    _pumpPins[i] = PIN_FirstPump + i;
    pinMode(PIN_FirstPump + i, OUTPUT);
    digitalWrite(PIN_FirstPump + i, DEFAULT_PUMP_STATE);
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
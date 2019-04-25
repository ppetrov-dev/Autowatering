#include <Arduino.h>
#include "GyverEncoder.h"
#include <EEPROMex.h>
#include <EEPROMVar.h>
#include "LCD_1602_RUS.h"
#include "OneButton.h"

#include "converters.h"
#include "settings.h"
#include "enums.h"

enum LcdCursorPosition _lcdCursorPosition;

OneButton button1(PIN_Button1, true);
OneButton button2(PIN_Button2, true);
Encoder _encoder(PIN_clk, PIN_dt, PIN_sw);
LCD_1602_RUS _lcd(0x27, 16, 2);

uint32_t _pumpTimersInMilliseconds[PUPM_AMOUNT];
uint32_t _pumpWorkTimesInMinutes[PUPM_AMOUNT];
uint32_t _pumpPauseTimesInMinutes[PUPM_AMOUNT];
bool _pumpStates[PUPM_AMOUNT];
byte _pumpPins[PUPM_AMOUNT];
int8_t _selectedDays, _selectedHours, _selectedMinutes;

int8_t _selectedPumpNumber;
bool _isPumping;

bool startFlag = true;

bool _isLcdOn = true;
unsigned long _lastEncoderUpdateTime;

void PrintSelectedPumpName()
{
  _lcd.setCursor(1, 0);
  _lcd.print("                ");
  _lcd.setCursor(1, 0);
  _lcd.print(PUMPS_NAMES[_selectedPumpNumber]);
}

void UpdateSelectedDaysHoursMinutes(uint32_t minutes)
{
  _selectedDays = floor((long)minutes / 1440);
  _selectedHours = floor((minutes - (long)_selectedDays * 1440) / 60);
  _selectedMinutes = floor(minutes - (long)_selectedDays * 1440 - (long)_selectedHours * 60);
}

uint32_t ConvertSelectedDaysHoursMinutesToMinutes()
{
  return ((long)_selectedDays * 1440 + (long)_selectedHours * 60 + _selectedMinutes);
}

void UpdateDataInMemoryForSelectedPump()
{
  EEPROM.updateLong(8 * _selectedPumpNumber, _pumpPauseTimesInMinutes[_selectedPumpNumber]);
  EEPROM.updateLong(8 * _selectedPumpNumber + 4, _pumpWorkTimesInMinutes[_selectedPumpNumber]);
}

void TurnLcdOn()
{
  _isLcdOn = true;
  _lcd.backlight();
}

bool IsLcdTimeoutExpired()
{
  return millis() - _lastEncoderUpdateTime >= ConvertSecondsToMilliseconds(Lcd_TIMEOUT_SECONDS);
}

void TurnLcdOffIfNeeded()
{
  if (!IS_LCD_AUTO_OFF || !_isLcdOn || !IsLcdTimeoutExpired())
    return;

  _isLcdOn = false;
  _lcd.noBacklight();
}

void TurnPumpOn(byte pumpNumber)
{
  digitalWrite(_pumpPins[pumpNumber], !DEFAULT_PUMP_STATE);
  _isPumping = true;

  if(START_TIMER_OPTION == WhenPumpIsOn)
    _pumpTimersInMilliseconds[pumpNumber] = millis();
    
    //Serial.println("Pump #" + String(i) + " ON");
}

void TurnPumpOff(byte pumpNumber)
{
  digitalWrite(_pumpPins[pumpNumber], DEFAULT_PUMP_STATE);
  _isPumping = false;

  if(START_TIMER_OPTION == WhenPumpIsOff)
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

  unsigned long currentMilliseconds = millis();
  for (byte pumpNumber = 0; pumpNumber < PUPM_AMOUNT; pumpNumber++)
  {
    if (_pumpPauseTimesInMinutes[pumpNumber] == 0 || _isPumping)
      continue;

    unsigned long pumpPauseTimeInMilliseconds = ConvertMinutesToMilliseconds(_pumpPauseTimesInMinutes[pumpNumber]);
    unsigned long passedTimeinMilliseconds = currentMilliseconds - _pumpTimersInMilliseconds[pumpNumber];
    if (passedTimeinMilliseconds < pumpPauseTimeInMilliseconds)
      continue;

    TurnPumpOn(pumpNumber);
  }
}

void CheckPumpsToWaterAndOffIfNeeded()
{
  if (!_isPumping)
    return;

  unsigned long currentMilliseconds = millis();
  for (byte pumpNumber = 0; pumpNumber < PUPM_AMOUNT; pumpNumber++)
  {
    if (_pumpWorkTimesInMinutes[pumpNumber] == 0 || _pumpStates[pumpNumber] != DEFAULT_PUMP_STATE)
      continue;

    unsigned long pumpWorkTimeInMilliseconds = ConvertMinutesToMilliseconds(_pumpWorkTimesInMinutes[pumpNumber]);
    unsigned long passedTimeinMilliseconds = currentMilliseconds - _pumpTimersInMilliseconds[pumpNumber];
    if (passedTimeinMilliseconds < pumpWorkTimeInMilliseconds)
      continue;

    TurnPumpOff(pumpNumber);
  }
}

void PrintArrowAndSetCursor(byte col, byte row)
{
  _lcd.setCursor(0, 0);
  _lcd.print(" ");
  _lcd.setCursor(7, 1);
  _lcd.print(" ");
  _lcd.setCursor(10, 1);
  _lcd.print(" ");
  _lcd.setCursor(13, 1);
  _lcd.print(":");
  _lcd.setCursor(col, row);
  _lcd.write(126);
}

void PrintDataAndUpdateArrowPosition()
{
  switch (_lcdCursorPosition)
  {
  case SelectPump:
    PrintArrowAndSetCursor(0, 0);
    UpdateDataInMemoryForSelectedPump();
    break;
  case SelectPauseDays:
  case SelectWorkDays:
    PrintArrowAndSetCursor(7, 1);
    break;
  case SelectPauseHours:
  case SelectWorkHours:
    PrintArrowAndSetCursor(10, 1);
    break;
  case SelectPauseMinutes:
  case SelectWorkMinutes:
    PrintArrowAndSetCursor(13, 1);
    break;
  }
  _lcd.setCursor(0, 1);
  if (_lcdCursorPosition < SelectWorkDays)
  {
    _lcd.print(L"PAUSE ");
    UpdateSelectedDaysHoursMinutes(_pumpPauseTimesInMinutes[_selectedPumpNumber]);
  }
  else
  {
    _lcd.print(L"WORK  ");
    UpdateSelectedDaysHoursMinutes(_pumpWorkTimesInMinutes[_selectedPumpNumber]);
  }
  _lcd.setCursor(8, 1);
  _lcd.print(_selectedDays);
  _lcd.print("d");
  _lcd.setCursor(11, 1);
  if (_selectedHours < 10)
    _lcd.print(0);
  _lcd.print(_selectedHours);
  _lcd.setCursor(14, 1);
  if (_selectedMinutes < 10)
    _lcd.print(0);
  _lcd.print(_selectedMinutes);
}

void ConstrainSelectedValues()
{
  if (_selectedDays > 9)
    _selectedDays = 9;
  if (_selectedMinutes > 59)
    _selectedMinutes = 0;
  if (_selectedHours > 59)
    _selectedHours = 0;
  if (_selectedMinutes < 0)
    _selectedMinutes = 59;
  if (_selectedHours < 0)
    _selectedHours = 59;
  if (_selectedDays < 0)
    _selectedDays = 9;
}

void ConstrainSelectedPumpNumber()
{
  if (_selectedPumpNumber > PUPM_AMOUNT - 1)
    _selectedPumpNumber = 0;
  if (_selectedPumpNumber < 0)
    _selectedPumpNumber = PUPM_AMOUNT - 1;
}

void UpdatePauseOrWorkTime()
{
  uint32_t newTime = ConvertSelectedDaysHoursMinutesToMinutes();
  bool isNewPauseTime = _lcdCursorPosition < SelectWorkDays;

  if (isNewPauseTime)
    _pumpPauseTimesInMinutes[_selectedPumpNumber] = newTime;
  else
    _pumpWorkTimesInMinutes[_selectedPumpNumber] = newTime;
}

void UpdateSelectedValues(int increment)
{
  if (_lcdCursorPosition == SelectPump)
  {
    _selectedPumpNumber += increment;
    ConstrainSelectedPumpNumber();
    uint32_t selectedPumpSavedMinutes = _pumpPauseTimesInMinutes[_selectedPumpNumber];
    UpdateSelectedDaysHoursMinutes(selectedPumpSavedMinutes);
    PrintSelectedPumpName();
  }
  else
  {
    if (_lcdCursorPosition == SelectPauseDays || _lcdCursorPosition == SelectWorkDays)
      _selectedDays += increment;
    else if (_lcdCursorPosition == SelectPauseHours || _lcdCursorPosition == SelectWorkHours)
      _selectedHours += increment;
    else if (_lcdCursorPosition == SelectPauseMinutes || _lcdCursorPosition == SelectWorkMinutes)
      _selectedMinutes += increment;

    ConstrainSelectedValues();

    UpdatePauseOrWorkTime();
  }
}

LcdCursorPosition GetNextLcdCursorPosition()
{
  switch (_lcdCursorPosition)
  {
  case SelectPump:
    return SelectPauseDays;
  case SelectPauseDays:
    return SelectPauseHours;
  case SelectPauseHours:
    return SelectPauseMinutes;
  case SelectPauseMinutes:
    return SelectWorkDays;
  case SelectWorkDays:
    return SelectWorkHours;
  case SelectWorkHours:
    return SelectWorkMinutes;
  case SelectWorkMinutes:
    return SelectPump;
  }
}

LcdCursorPosition GetPreviousLcdCursorPosition()
{
  switch (_lcdCursorPosition)
  {
  case SelectPump:
    return SelectWorkMinutes;
  case SelectPauseDays:
    return SelectPump;
  case SelectPauseHours:
    return SelectPauseDays;
  case SelectPauseMinutes:
    return SelectPauseHours;
  case SelectWorkDays:
    return SelectPauseMinutes;
  case SelectWorkHours:
    return SelectWorkDays;
  case SelectWorkMinutes:
    return SelectWorkHours;
  }
}

void HandleEncoderTick()
{
  _encoder.tick();

  if (_encoder.isTurn())
  {
    _lastEncoderUpdateTime = millis();

    if (_isLcdOn)
    {
      if (_encoder.isRight())
        _lcdCursorPosition = GetNextLcdCursorPosition();
      else if (_encoder.isLeft())
        _lcdCursorPosition = GetPreviousLcdCursorPosition();

      if (_encoder.isRightH())
        UpdateSelectedValues(1);
      else if (_encoder.isLeftH())
        UpdateSelectedValues(-1);

      PrintDataAndUpdateArrowPosition();
    }
    else
      TurnLcdOn();
  }
}
void ForceWatering(unsigned int pin, bool isOn)
{
  digitalWrite(pin, isOn);
}

void OnLongPressButton1Start()
{
  ForceWatering(_pumpPins[0], HIGH);
}
void OnLongPressButton1Stop()
{
  ForceWatering(_pumpPins[0], LOW);
}
void OnLongPressButton2Start()
{
  ForceWatering(_pumpPins[1], HIGH);
}
void OnLongPressButton2Stop()
{
  ForceWatering(_pumpPins[1], LOW);
}

void setup()
{
  pinMode(PIN_Button1, INPUT_PULLUP);
  pinMode(PIN_Button2, INPUT_PULLUP);

  button1.attachLongPressStart(OnLongPressButton1Start);
  button1.attachLongPressStop(OnLongPressButton1Stop);
  button2.attachLongPressStart(OnLongPressButton2Start);
  button2.attachLongPressStop(OnLongPressButton2Stop);

  // --------------------- КОНФИГУРИРУЕМ ПИНЫ ---------------------
  for (byte i = 0; i < PUPM_AMOUNT; i++)
  {                                                       // пробегаем по всем помпам
    _pumpPins[i] = PIN_FirstPump + i;                     // настраиваем массив пинов
    pinMode(PIN_FirstPump + i, OUTPUT);                   // настраиваем пины
    digitalWrite(PIN_FirstPump + i, !DEFAULT_PUMP_STATE); // выключаем от греха
  }
  // --------------------- ИНИЦИАЛИЗИРУЕМ ЖЕЛЕЗО ---------------------
  Serial.begin(9600);

  _lcd.init();
  _lcd.backlight();
  _lcd.clear();
  //_encoder.setStepNorm(1);
  //attachInterrupt(0, encISR, CHANGE);
  _encoder.setType(ENCODER_TYPE);
  if (IS_ENCODER_REVERSED)
    _encoder.setDirection(REVERSE);

  // --------------------- СБРОС НАСТРОЕК ---------------------
  if (!digitalRead(PIN_sw))
  { // если нажат энкодер, сбросить настройки до 1
    _lcd.setCursor(0, 0);
    _lcd.print("Reset settings");
    //  EEPROM.clear();
  }
  while (!digitalRead(PIN_sw))
    ;           // ждём отпускания кнопки
  _lcd.clear(); // очищаем дисплей, продолжаем работу

  // --------------------------- НАСТРОЙКИ ---------------------------
  // в ячейке 1023 должен быть записан флажок, если его нет - делаем (ПЕРВЫЙ ЗАПУСК)
  if (EEPROM.read(1023) != 5)
  {
    EEPROM.writeByte(1023, 5);

    // для порядку сделаем 1 ячейки с 0 по 500
    for (byte i = 0; i < 500; i += 4)
    {
      EEPROM.writeLong(i, 0);
    }
  }

  for (byte i = 0; i < PUPM_AMOUNT; i++)
  {                                                          // пробегаем по всем помпам
    _pumpPauseTimesInMinutes[i] = EEPROM.readLong(8 * i);    // читаем данные из памяти. На чётных - период (ч)
    _pumpWorkTimesInMinutes[i] = EEPROM.readLong(8 * i + 4); // на нечётных - полив (с)

    // вырубить все помпы
    _pumpStates[i] = DEFAULT_PUMP_STATE;
  }

  // ---------------------- ВЫВОД НА ДИСПЛЕЙ ------------------------
  PrintSelectedPumpName();
  PrintDataAndUpdateArrowPosition();
}

void loop()
{
  button1.tick();
  button2.tick();

  HandleEncoderTick();
  CheckPumpsToWaterAndOnIfNeeded();
  CheckPumpsToWaterAndOffIfNeeded();
  TurnLcdOffIfNeeded();
}
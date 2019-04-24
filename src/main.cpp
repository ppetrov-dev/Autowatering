#include <Arduino.h>
#include "GyverEncoder.h"
#include <EEPROMex.h>
// #include <EEPROMVar.h>
#include "LCD_1602_RUS.h"
#include "OneButton.h"

#define IS_Lcd_AUTO_OFF 1
#define Lcd_TIMEOUT_SECONDS 60
#define ENCODER_TYPE 0 // тип энкодера (0 или 1). Если энкодер работает некорректно (пропуск шагов), смените тип
#define IS_ENCODER_REVERSED 0

#define DEFAULT_pumpStates 0 // реле: 1 - высокого уровня (или мосфет), 0 - низкого
#define TIMER_START 1         // 1 - отсчёт периода с момента ВЫКЛЮЧЕНИЯ помпы, 0 - с момента ВКЛЮЧЕНИЯ помпы

static const wchar_t *PUMPS_NAMES[] = {
    L"Pump 1",
    L"Pump 2"};

#define PIN_clk 2
#define PIN_dt 3
#define PIN_sw 4
#define PIN_Button1 5
#define PIN_Button2 6
#define PUPM_AMOUNT 2
#define PIN_FirstPump 7

OneButton button1(PIN_Button1, true);
OneButton button2(PIN_Button2, true);
Encoder _encoder(PIN_clk, PIN_dt, PIN_sw);
LCD_1602_RUS _lcd(0x27, 16, 2);

uint32_t _pumpTimers[PUPM_AMOUNT];
uint32_t _pumpWorkingTimes[PUPM_AMOUNT];
uint32_t _pumpPauseTimes[PUPM_AMOUNT];
bool _pumpStates[PUPM_AMOUNT];
byte _pumpPins[PUPM_AMOUNT];
int8_t _selectedDays, _selectedHours, _selectedMinutes;

int8_t _currentState;
int8_t _currentPump;
bool _isPumping;

bool startFlag = true;

bool _isLcdOn = true;
unsigned int _disableLcdTimer;

// вывести название реле
void drawLabels()
{
  _lcd.setCursor(1, 0);
  _lcd.print("                ");
  _lcd.setCursor(1, 0);
  _lcd.print(PUMPS_NAMES[_currentPump]);
}

void ConvertMinutesToDaysHoursMinutes(uint32_t minutes)
{
  _selectedDays = floor((long)minutes / 1440);
  _selectedHours = floor((minutes - (long)_selectedDays * 1440) / 60);
  _selectedMinutes = floor(minutes - (long)_selectedDays * 1440 - (long)_selectedHours * 60);
}

uint32_t ConvertDaysHoursMinutesToMinutes()
{
  return ((long)_selectedDays * 1440 + (long)_selectedHours * 60 + _selectedMinutes);
}

void UpdateDataInMemory()
{
  EEPROM.updateLong(8 * _currentPump, _pumpPauseTimes[_currentPump]);
  EEPROM.updateLong(8 * _currentPump + 4, _pumpWorkingTimes[_currentPump]);
}

void TurnLcdOn()
{
  _isLcdOn = true;
  _disableLcdTimer = millis();
  _lcd.backlight();
}
void TurnLcdOff()
{
  if (IS_Lcd_AUTO_OFF && _isLcdOn && millis() - _disableLcdTimer >= Lcd_TIMEOUT_SECONDS * 1000)
  {
    _isLcdOn = false;
    _lcd.noBacklight();
  }
}

void periodTick()
{
  for (byte i = 0; i < PUPM_AMOUNT; i++)
  { // пробегаем по всем помпам
    if (startFlag ||
        (_pumpPauseTimes[i] > 0 && millis() - _pumpTimers[i] >= _pumpPauseTimes[i] * 1000 *60 
        && (_pumpStates[i] != DEFAULT_pumpStates) && !_isPumping))
    {
      _pumpStates[i] = DEFAULT_pumpStates;
      digitalWrite(_pumpPins[i], DEFAULT_pumpStates);
      _pumpTimers[i] = millis();
      _isPumping = true;
      //Serial.println("Pump #" + String(i) + " ON");
    }
  }
  startFlag = false;
}

void FlowTick()
{
  for (byte i = 0; i < PUPM_AMOUNT; i++)
  { // пробегаем по всем помпам
    if (_pumpWorkingTimes[i] > 0 && millis() - _pumpTimers[i] >= _pumpWorkingTimes[i] * 1000 *60
    && (_pumpStates[i] == DEFAULT_pumpStates))
    {
      _pumpStates[i] = !DEFAULT_pumpStates;
      digitalWrite(_pumpPins[i], !DEFAULT_pumpStates);
      if (TIMER_START)
        _pumpTimers[i] = millis();
      _isPumping = false;
      //Serial.println("Pump #" + String(i) + " OFF");
    }
  }
}

// отрисовка стрелки и двоеточий
void DrawArrows(byte col, byte row)
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

// изменение позиции стрелки и вывод данных
void changeSet()
{
  switch (_currentState)
  {
  case 0:
    DrawArrows(0, 0);
    UpdateDataInMemory();
    break;
  case 1:
    DrawArrows(7, 1);
    break;
  case 2:
    DrawArrows(10, 1);
    break;
  case 3:
    DrawArrows(13, 1);
    break;
  case 4:
    DrawArrows(7, 1);
    break;
  case 5:
    DrawArrows(10, 1);
    break;
  case 6:
    DrawArrows(13, 1);
    break;
  }
  _lcd.setCursor(0, 1);
  if (_currentState < 4)
  {
    _lcd.print(L"PAUSE ");
    ConvertMinutesToDaysHoursMinutes(_pumpPauseTimes[_currentPump]);
  }
  else
  {
    _lcd.print(L"WORK  ");
    ConvertMinutesToDaysHoursMinutes(_pumpWorkingTimes[_currentPump]);
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
// тут меняем номер помпы и настройки
void changeSettings(int increment)
{
  if (_currentState == 0)
  {
    _currentPump += increment;
    if (_currentPump > PUPM_AMOUNT - 1)
      _currentPump = PUPM_AMOUNT - 1;
    if (_currentPump < 0)
      _currentPump = 0;
    ConvertMinutesToDaysHoursMinutes(_pumpPauseTimes[_currentPump]);
    drawLabels();
  }
  else
  {
    if (_currentState == 1 || _currentState == 4)
    {
      _selectedDays += increment;
    }
    else if (_currentState == 2 || _currentState == 5)
    {
      _selectedHours += increment;
    }
    else if (_currentState == 3 || _currentState == 6)
    {
      _selectedMinutes += increment;
    }
    if (_selectedDays > 9)
    {
      _selectedDays = 9;
    }
    if (_selectedMinutes > 59)
    {
      _selectedMinutes = 0;
    }
    if (_selectedHours > 59)
    {
      _selectedHours = 0;
    }
    if (_selectedMinutes < 0)
    {
      _selectedMinutes = 59;
    }
    if (_selectedHours < 0)
    {
      _selectedHours = 59;
    }
    if (_selectedDays < 0)
    {
      _selectedDays = 9;
    }
    if (_currentState < 4)
      _pumpPauseTimes[_currentPump] = ConvertDaysHoursMinutesToMinutes();
    else
      _pumpWorkingTimes[_currentPump] = ConvertDaysHoursMinutesToMinutes();
  }
}
void EncoderTick()
{
  _encoder.tick(); // отработка энкодера

  if (_encoder.isTurn())
  { // если был совершён поворот
    if (_isLcdOn)
    {
      _disableLcdTimer = millis(); // сбросить таймаут дисплея
      if (_encoder.isRight())
      {
        if (++_currentState >= 7)
          _currentState = 6;
      }
      else if (_encoder.isLeft())
      {
        if (--_currentState < 0)
          _currentState = 0;
      }

      if (_encoder.isRightH())
        changeSettings(1);
      else if (_encoder.isLeftH())
        changeSettings(-1);

      changeSet();
    }
    else
    {
      TurnLcdOn(); // включить дисплей
    }
  }
}
void ForcePumping(unsigned int pin, bool isOn)
{
  digitalWrite(pin, isOn);
}

void OnLongPressButton1Start()
{
  ForcePumping(_pumpPins[0], HIGH);
}
void OnLongPressButton1Stop()
{
  ForcePumping(_pumpPins[0], LOW);
}
void OnLongPressButton2Start()
{
  ForcePumping(_pumpPins[1], HIGH);
}
void OnLongPressButton2Stop()
{
  ForcePumping(_pumpPins[1], LOW);
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
  {                                                        // пробегаем по всем помпам
    _pumpPins[i] = PIN_FirstPump + i;                      // настраиваем массив пинов
    pinMode(PIN_FirstPump + i, OUTPUT);                    // настраиваем пины
    digitalWrite(PIN_FirstPump + i, !DEFAULT_pumpStates); // выключаем от греха
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
  {                                                    // пробегаем по всем помпам
    _pumpPauseTimes[i] = EEPROM.readLong(8 * i);       // читаем данные из памяти. На чётных - период (ч)
    _pumpWorkingTimes[i] = EEPROM.readLong(8 * i + 4); // на нечётных - полив (с)

    if (DEFAULT_pumpStates) // вырубить все помпы
      _pumpStates[i] = 0;
    else
      _pumpStates[i] = 1;
  }

  // ---------------------- ВЫВОД НА ДИСПЛЕЙ ------------------------
  drawLabels();
  changeSet();
}

void loop()
{
  button1.tick();
  button2.tick();

  EncoderTick();
  periodTick();
  FlowTick();
  TurnLcdOff();
}
#include "enums.h"

#ifndef settingsH
#define settingsH

#define IS_LCD_AUTO_OFF 1
#define Lcd_TIMEOUT_SECONDS 60
#define ENCODER_TYPE 0 // тип энкодера (0 или 1). Если энкодер работает некорректно (пропуск шагов), смените тип
#define IS_ENCODER_REVERSED 0
#define DEFAULT_PUMP_STATE 0 // реле: 1 - высокого уровня (или мосфет), 0 - низкого

#define PIN_clk 2
#define PIN_dt 3
#define PIN_sw 4
#define PIN_Button1 5
#define PIN_Button2 6
#define PUPM_AMOUNT 2
#define PIN_FirstPump 7

static const wchar_t *PUMPS_NAMES[] = {
    L"Pump 1",
    L"Pump 2"};

static const enum StartTimerOption START_TIMER_OPTION = WhenPumpIsOff;

#endif
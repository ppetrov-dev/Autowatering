#ifndef settingsH
#define settingsH

#include "enums.h"

#define DEBUG 0

#define IS_LCD_AUTO_OFF 1
#define IS_ENCODER_REVERSED 1
#define IS_PARALLEL_WATERING_DISABLED 1

#define FORCEDLY_STARTED_PUMP_SECONDS 15
#define Lcd_TIMEOUT_SECONDS 60
#define ENCODER_TYPE 1 // 0 or 1, change the type If incorrect working

#define PIN_EncoderClk 2
#define PIN_EncoderDt 3
#define PIN_EncoderSw 4

#define PIN_Button1 5
#define PIN_Button2 6

#define PIN_FirstPump 7
#define PUPM_AMOUNT 2

#define RELAY_TYPE HighLevel

#endif
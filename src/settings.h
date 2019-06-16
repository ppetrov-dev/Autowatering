#ifndef settingsH
#define settingsH

#include "enums.h"

#define DEBUG 1

#define IS_LCD_AUTO_OFF 1
#define IS_ENCODER_REVERSED 1
#define IS_PARALLEL_WATERING_DISABLED 1

#define DISTANCE_TO_BOTTOM_CM 30
#define ACCEPTABLE_TIME_OFFSET_SECONDS 300
#define FORCEDLY_STARTED_PUMP_SECONDS 15
#define LCD_TIMEOUT_SECONDS 60
#define ENCODER_TYPE 1 // 0 or 1, change the type If incorrect working

#define I2C_LcdAddress 0x27
// #define DS3231_ADDRESS	      0x68 //I2C Slave address - from Sodaq_DS3231.cpp

#define PIN_EncoderClk 2
#define PIN_EncoderDt 3
#define PIN_EncoderSw 4

#define PIN_Button1 5
#define PIN_Button2 6

#define PIN_FirstPump 7
#define PUMP_AMOUNT 2

#define PIN_UltrasonicSensorTrigger A2
#define PIN_UltrasonicSensorEcho 12

#define RELAY_TYPE HighLevel

#endif
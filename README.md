# Autowatering - Arduino DIY project

The project based on Alex Gyvyer' project (https://github.com/AlexGyver/Auto-Pumps) for plants autowatering. All the code was modified. For programming I use VS Code + Platform IO (https://platformio.org/).

Potentially many pumps could be added but I assembled the circuit and designed the box (https://www.thingiverse.com/thing:3607432) for 2 pumps. But this box was too small to put there all cables and I used another one from AliExpress. Also I've added 2 buttons for force starting of the pumps.

* [The main improvements](#The-main-improvements)
* [What you need to repeat](#what-you-need-to-repeats)
* [How to connect](#how-to-connect)
* [Versions](#versions)
* [What next](#what-next)

![alt text][device]

<a id="The-main-improvements"></a>
## The main improvements 
* Object-oriented and readable code, as much as possible
* Separated file for settings (pins, timeout time etc)
* Separated menu in the system interface to set working and wating times of pumps
* Saving data in Arduino memory (EEPROM) happens when an user leaves menu settings and if changes are
* Minimum time for a pump to wait is 1 minute, to work - 1 second 
* Maximum time for a pump to wait is almost 10 days (-1 minute), to work - almost 1 day (-1 second) 
* Forse start/stop pumps by buttons commands (double click, hold press/release)
* Different timers display remaining time to change a pump state, force start timer in seconds 

<a id="what-you-need-to-repeats"></a>
## What you need to repeat
![alt text][whatYouNeed]
* Arduino Nano
* 2 channel relay
* Lcd1602 module with I2C
* Rotary encoder
* Buttons x2
* Pumps (5V) x2 **(than I used 12V pumps + step up converter)**
* Power supply (5V)
* Wires to connect, circuit board
* Box
* Diodes 1N4007 x2 (optional - depends on type of your relay)
* Сapacitor 470uf or more (optional)
* Check valves x2 (optional)
* Water hose (optional)

<a id="how-to-connect"></a>
## How to connect
See scheme of original project (https://github.com/AlexGyver/Auto-Pumps). I've done the same but I have 2 channel relay without diodes inside and I had to add them to the pumps outs. It looks like this:
![alt text][howToConnectDiodes]

Also I added capacitor in the scheme and connected it to arduino outs:
![alt text][howToConnectCapacitor]

And I have different pins to connect modules you can find them in "settings.h" file:
```C++
#define I2C_LcdAddress 0x27

#define PIN_EncoderClk 2
#define PIN_EncoderDt 3
#define PIN_EncoderSw 4

#define PIN_Button1 5
#define PIN_Button2 6

#define PIN_FirstPump 7
#define PUMP_AMOUNT 2
```
I use 2 pumps (D7 and D8 outs in Arduiono) but you can add more. See code how to implement that. Also you can remove buttons if you don't need them and use their pins to connect extra pumps.  

<a id="versions"></a>
## Versions
version 1.0 - current<br/>
[version 1.1](https://github.com/ppetrov-dev/Autowatering/tree/version-1.1) (with RTC module)

<a id="what-next"></a>
## What next
I don't have any certain plans for the project. I'd add a soil moisture sensor or/and real-time clocks module. Most likely it will be connected with the Internet. I'll create an account in Twitter for my plants and they will post how comfortable they live. Will see.

[whatYouNeed]: https://github.com/ppetrov-dev/Autowatering/raw/version-1.0/Screenshots/WhatYouNeed.png
[howToConnectDiodes]: https://github.com/ppetrov-dev/Autowatering/raw/version-1.0/Screenshots/HowToConnectDiodes.jpg
[howToConnectCapacitor]: https://github.com/ppetrov-dev/Autowatering/raw/version-1.0/Screenshots/HowToConnectCapacitor.png
[device]: https://github.com/ppetrov-dev/Autowatering/raw/version-1.0/Screenshots/device.png

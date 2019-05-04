# Autowatering - Arduino DIY project

The project based on Alex Gyvyer' project (https://alexgyver.ru/auto-pumps/) for plants autowatering. All the code was modified. For programming I use VS Code + Platform IO (https://platformio.org/).

Potentially many pumps could be added but I assembled the circuit and designed the box (https://www.thingiverse.com/thing:3607432) for 2 pumps. Also I've added 2 buttons for force starting of the pumps.

## The main improvements:
* Object-oriented and readable code, as much as possible
* Separated file for settings (pins, timeout time etc)
* Separated menu in the system interface to set working and wating times of pumps
* Saving data in Arduino memory (EEPROM) happens when an user leaves menu settings and if changes are
* Minimum time for a pump to wait is 1 minute, to work - 1 second 
* Maximum time for a pump to wait is almost 10 days (-1 minute), to work - almost 1 day (-1 second) 
* Forse start/stop pumps by buttons commands (double click, hold press/release)
* Different timers display remaining time to change a pump state, force start timer in seconds 

## What next
I don't have any certain plans for the project. I'd add a soil moisture sensor. Most likely it will be connected with the Internet. I'll create an account in Twitter for my plants and they will post how comfortable they live. Will see.

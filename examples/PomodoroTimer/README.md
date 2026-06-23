# Podomoro Timer Example

This simple example for the "Cheeseboard" unit (a NodeMCU ESP8266 device)

This will run different timers to perform different 'Pomodoro' style work timers.

These can be:

Traditional:    25 mins focus,  5 mins break 

Deep Work:      50 mins focus,  10 mins break

Sprint:         10 mins focus,  3 mins break

Focus is Blue, Break is Green    

The unit will start when the main button is pressed. Short press = start and pause. Long press (1 sec) will stop the unit

The rotary encoder chooses which type of timer is implemented


## Build instructions

1. Install pre-requisites as described in the main CheeseBoard README.md file
2. Open Test.ino in the Arduino IDE
3. Click build
4. Connect the CheeseBoard with the connector on the NodeMCU
5. Upload

## To Do:
Remove blue LED from NodeMCU - it flashes every time OLED is updated!
# CheeseBoard Library

This library contains components for use with the *CheeseBoard: Cheddar* and variants, with examples which demonstrate the use of the various devices on the board:

* WiFi
* OLED screen
* RGB LEDs
* Rotary encoder / push button
* HC12 wireless serial module (optional extra)

## Setting Up the Arduino IDE

* Install Arduino IDE (this process tested with version 1.8.5 on Windows/Linux)
* Visit https://github.com/esp8266/Arduino
    * Scroll down and copy the "Boards manager link" for the stable release
* In the Arduino IDE, go to: File [menu] -> Preferences [menuitem]
    * Paste the link into the "Additional Board Manager URLs" box 
    * If there's already something there, add a new line with the link
    * Click the OK [button]
* Tools [menu] -> Board [menu item] -> Boards Manager [menu item]
    * Wait for Downloading Platforms Index to download
    * Search for "esp8266"
    * Install latest version of "esp8266 by Esp8266 Community"
    * Click the OK [button]
* Sketch [menu] -> Include Library [menu item] -> Manager Libraries... [menu item]
* Search for and install the following libraries:
    * Adafruit NeoPixel library by Adafruit (install using IDE library manager)
    * Encoder library by Paul Stoffregen (install using IDE library manager)
    * Time library by Michael Margolis (install using IDE library manager, search for "Timekeeping")
    * U8g2 library by Oliver Kraus (install using IDE library manager)
    * Mutila lirary by Matthew Gates (install using IDE library manager)
* Visit https://github.com/matthewg42/EspApConfigurator click "Clone or download" and choose "Download ZIP"
    * Sketch [menu] -> Include Library [menu item] -> Add .ZIP library... [menu item]
    * Find the .ZIP file you downloaded and select it, click OK
* Visit https://github.com/curiouselectric/CheeseBoard_Software (this repository) click "Clone or download" and choose "Download ZIP"
    * Sketch [menu] -> Include Library [menu item] -> Add .ZIP library... [menu item]
    * Find the .ZIP file you downloaded and select it, click OK

### Installing the Serial Drivers

Your computer must have the CP210x driver installed to communicate with the NodeMCU over USB.

#### Linux

* Mainstream Linux distributions (e.g. Fedora 27) tend to come with the driver pre-installed
* Sourcecode for the drive can be downloaded from [the Silican Labs website](https://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers) if required
* When the driver is installed and the NodeMCU is connected to the computer, your system should create a device node with the path */dev/ttyUSB* with a numerical suffix. The suffix starts at 0 and increments for each serial USB device connected, for example */dev/ttyUSB0*, */dev/ttyUSB1* and so on - this is the value of the port setting to be used in the Ardino IDE when uploading
* Note: You must have read/write permissions for the device node in order to communicte with the NodeMCU. This may require your user to be added to an auxillary group. For example, on Fedora, the /dev/ttyUSB nodes have the group permission set to *dialout*, so you should ensure your user is in that group. After adding your user to the group, you may need to log in for the new setting to take effect

#### Mac OSX

The driver for the NodeMCU's serial interface can be download from the Silican Labs website. Depending on the version of MacOS X you are running, will need different version of the driver.

* For MacOS X 10.11 and later, install the version of the driver labelled "Download VCP" from [the main download page](https://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers)
* For earlier releases of MacOS X, use the legacy driver [from this page](https://www.silabs.com/community/interface/knowledge-base.entry.html/2017/01/10/legacy_os_softwarea-bgvU)
* When the driver is installed and the NodeMCU is connected to the computer using the MicroUSB cable, a device node should be created with the path */dev/cu.SLAB_USBtoUART*. This is the name used in the *port* setting in the Arduino IDE when uploading
* Note: You must have read/write permissions for the device node in order to communicte with the NodeMCU

#### Windows

On Windows, check to see if the driver is installed as follows:

1. Connect the NodeMCU to your computer using a Micro-USB cable (connect the cable directly to the NodeMCU, not to the secondary (power only) MicroUSB socket on the CheeseBoard itself)
2. Open Device Manager, and expand the group *Ports (COM & LPT)*
3. If there is no entry for the CP210x driver, or it has an exclamation mark over the icon, you must install the driver
    * You can download the driver from [the Silican Labs website](https://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers)
    * After installing the drive, unplug the USB cable and then plug it in again
4. Once the driver is installed, the device's port identifier can be see in Device Manager under the *Ports (COM & LPT)* group - this idenfifier (e.g. *COM3*) is what is used in the *ports* setting in the Arduino IDE when uploading

## Building and Uploading an Example Sketch

First set up the Arduino IDE as described above and install the CP210x driver if required. Then connect the NodeMCU to your computer using a Micro USB cable connected directly to the NodeMCU (not the power-only USB socket on the CheeseBoard PCB). Find the *port* setting for the NodeMCU, as described in "Installing the Serial Drivers" above.

* Open the HardwareTest example: File [menu] -> Examples [menu item] -> Examples From Custom Libraries [menu item] -> CheeseBoard [menu item] -> HardwareTest [menu item]
* Choose the NodeMCU board: Tools [menu] -> Board ... [menu item] -> NodeMCU 1.0 (ESP-12E Module)
* Click Verify [toolbar button] to build the example - this might take a while
* Connect the USB cable direcly to the NodeMCU module (not to the other USB connector!)
* Select the correct serial port with Tools [menu] -> Port [menu item] -> *port for NodeMCU*
* Select high speed with Tools [menu] -> Upload Speed [menu item] -> 921600
* Click Upload [toolbar button]. Upload can take a little while compared to a regular Arduino - there is much more data being sent!

If you have trouble with the upload, try adjusting the speed and trying again (with some cables the highest speeds don't work so well).

Each example has a README.md file which gives an overview of the functionality and how to use it.

## Library Overview

The library defines the following objects and classes:

* CbLeds - Global instance of CbNeoPixel class - for controlling the RGB LEDs (inherits from Adafruit_NeoPixel, adjusting Millis() as necessary to avoid time drift)
* CbOledDisplay - Global instance of CbOledDisplayClass - U2g2 object which adds justified wrapped text 
* CbRotaryInput - Global instance of CbRotaryInputClass for interactive with the rotary encoder + button inputs
* CbHC12 - Global instance of SoftwareSerial-based class for using the HC12 
* GfxItem - base class for high-level objects which can be drawn on CbOledDisplay
* GfxSSIDListBox - a list box for displaying WiFi network information
* GfxNetInfo - an item for the GfxSSIDListBox
* GfxScene - experimental collection of GfxItem objects
* GfxSignalStrength - little signal strength meter for use in GfxNetInfo
* GfxStringListBox - generic listbox for strings (needs completion)
* GfxTextBox - show text in a box with optional border

## Notes About the CheeseBoard Library

The CheeseBoard library and examples use the Mutila library for various functions and classes. To avoid confusion, I'll mention a few here which you may notice in the examples:

0. The CheeseBoard library and examples are still under development at time of writing. Some of the code is a bit ugly and/or incomplete

1. The macros DB, DBLN abd DBF act like Serial.print, Serial.println and Serial.printf respectively. These only produce output if DEBUG is defined, allowing quick enabling / disabling of debugging output on serial when building with the Makefile. To enable debugging output from the IDE, simply add this line at the top of your sketch:

```cpp
#define DEBUG 1
```

2. When sending color data to RGB LEDs, interrupts are disabled. This is because the timing of the signal is critical. However, this causes an unwanted side-effect - the counter which the millis() function returns is not updated while interrupts are disabled. With heavy RGB LED use, this causes the millis() return value to be less than excpected. With very frequent updates to RGB LEDs, this can be quite pronounced. Mutila implements Millis(), which does the same thing as millis(), but allows for a correction to be applied, which is done when the CbNeoPixel class is used (CbLeds is a global instance of CbNeoPixel).

3. Mutila classes for hardware devices like buttons are all implemented with a timeslice approach. That is, they require an update() function to be called frequently to maintain their state. The idea behind this is to avoid using interrupts wherever possible in order to prevent conflicts with other libraries.

4. In the examples, global instances of classes are defined in files whose names are the same as the global object, e.g. Button.h and Button.cpp should define a global object called Button. If the class of that object is also defined in these files, the class name may be the same as the object name with an underscore or "Class" as a suffix.

5. In the CheeseBoard examples, I include function prototypes before any function definitions. This is so I can have setup() and loop() at the top of the main source file, but still build with a Makefile. They are not strictly necessary for the IDE build.

6. EspApConfigurator is my own re-implementation of venerable WiFi Manager library. It doesn't block like WiFi Manager, and has a better (IMO) persistent settings system. At time of writing, EspApConfigurator is not complete - multi-page setup mode and custom themed settings pages are still to be implemented. However, single page configuration mode works just fine, which is what we use in the CheeseBoard examples.

More detailed Doxygen-generated API documentation for the library can be found here: 

* https://matthewg42.github.io/Mutila/
* https://matthewg42.github.io/EspApConfigurator/
* https://matthewg42.github.io/CheeseBoard/

## Selecting Cheeseboard Version

Select your hardware by editing the CheeseboardConfig.h file, specifying the ???????Config.h file for your hardware and version. For example, the prototype CheeseBoard: Cheddar uses the config file CheddarV0Config.h, so we edit the CheeseboardConfig.h to contain:

```cpp
#pragma once

#include "CheddarV0Config.h"
```

At time of writing, this is the default since this is the only board which has been made. This mechanism may change in future releases.

## Future

* Better method of selecting hardware
* Make sure all dependent libs are available in IDE library manager - including CheeseBoard
* Light metronome example
* HC12 AT mode configuration utility example example


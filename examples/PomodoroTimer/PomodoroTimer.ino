

#include <Arduino.h>

// Include all dependent libraries so the Arduino IDE knows to add
// the library paths into the build. Make sure all these libraries
// are installed on your system
#include <U8g2lib.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_NeoPixel.h>

#define ENCODER_DO_NOT_USE_INTERRUPTS
#include <Encoder.h>
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>

// Some includes from the Mutila library which is used by the
// CheeseBoard library and others which I use
#include <MutilaDebug.h>
#include <Millis.h>

// Include CheeseBoard headers for the components we'll be using in
// this example
#include <CbLeds.h>
#include <CbOledDisplay.h>
#include <CbRotaryInput.h>

#include "EspID.h"
#include "Config.h"

// Function prototypes.  For the Arduino IDE you don't need these, but they
// make it possible to build with the Makefile approach without having to put
// functions in the file before they are referenced by other functions.
void setup();
void loop();
void buttonCb(uint16_t durationMs);
void display();
void rotaryCb(int8_t diff, int32_t value);

// Global variables
bool PreviousButton = false;
int32_t RotaryValue = 0;
int32_t PreviousRotaryValue = -1;

bool timerRunning = false;       // This is high if the timer is running
bool timerPaused = false;        // This is high if the timer is running
uint32_t podomoroTime = 0;       // For the main timer
uint32_t updateDisplayTime = 0;  // For updating the display

int8_t timerPtr = 2;  // Points to the type of timer used
uint32_t focus_time_msec;
uint32_t break_time_msec;

struct timerInfoLayout {
  String timer_type;
  uint32_t focus_time;
  uint32_t break_time;
};

#define timerPtrMax 3  // This must be set to number of times within timerInfo ****TO DO - sort out auto calculation of this****

timerInfoLayout timerInfo[timerPtrMax]{
  { "Traditional", 25, 5 },
  { "Deep Work", 50, 10 },
  { "Sprint", 10, 3 }
};


void setup() {
  Serial.begin(115200);
  delay(100);
  DBLN(F("\n\nCheeseBoard Pomodoro Firmware\n"));

  EspID.begin();

  // Switch OFF blue LEDs
  pinMode(D0, OUTPUT);     // Set GPIO16 (D0) to output
  digitalWrite(D0, HIGH);  // Turn the blue LED OFF (Active Low)

  // Init CbLeds
  CbLeds.begin();

  // Init the RotaryInput object
  CbRotaryInput.begin(buttonCb, rotaryCb);
  CbOledDisplay.begin();

  CbLeds.clear();
  CbLeds.show();

  // Update the OLED display
  display();

  DBLN(F("E:setup"));
}

void loop() {

  // Check rotary
  CbRotaryInput.update();
  if (PreviousRotaryValue != RotaryValue) {
    if (RotaryValue > PreviousRotaryValue) {
      timerPtr++;
      if (timerPtr >= timerPtrMax) {
        timerPtr = 0;
      }
    } else {
      timerPtr--;
      if (timerPtr < 0) {
        timerPtr = timerPtrMax - 1;
      }
    }
// Convert the struct values in a locally usable mSec value - testing converts seconds, otherwise convert mins
#ifdef TESTING
    focus_time_msec = (timerInfo[timerPtr].focus_time * 1000);  // Convert to mS TESTING ONLY
    break_time_msec = (timerInfo[timerPtr].break_time * 1000);  // Convert to mS TESTING ONLY
#else
    focus_time_msec = (timerInfo[timerPtr].focus_time * 1000 * 60);  // Converts min to mS
    break_time_msec = (timerInfo[timerPtr].break_time * 1000 * 60);  // Converts min to mS
#endif
    PreviousRotaryValue = RotaryValue;
    display();
  }

  //Check button
  bool button = CbRotaryInput.buttonPushed();
  if (button != PreviousButton) {
    PreviousButton = button;
    display();
  }

  // Check the timers (only if running) and update OLED and the LEDs
  if (timerRunning && (millis() > (updateDisplayTime + DISPLAY_UPDATE))) {
    if (!timerPaused) {
      podomoroTime += (millis() - updateDisplayTime);  // Add to the timer.
    }
    updateDisplayTime = millis();
    // Sort out the LEDs
    if (podomoroTime < (focus_time_msec)) {
      setLEDs(0x0000FF);  // Set BLUE
    } else if (podomoroTime < (focus_time_msec) + (break_time_msec)) {
      setLEDs(0x00FF00);  // Set GREEN
    } else {
      podomoroTime = 0;  // Start the loop again
    }
    display();
  }
}

void display() {
  // Update the OLED display
  String text = F("Pomodoro Timer\n");
  text += timerInfo[timerPtr].timer_type;
  text += F("\nFocus: ");
  text += timerInfo[timerPtr].focus_time;
  text += F("  Break: ");
  text += timerInfo[timerPtr].break_time;
  text += F("\n");
  if (timerPaused) {
    text += F("Paused");
  } else {
    text += timerRunning ? F("Running") : F("Stopped");
  }
  text += F("\n");
  if (timerRunning) {
    // Show the time left in Focus or Break:
    if (podomoroTime < (focus_time_msec)) {
      text += F("FOCUS: ");
      text += ((podomoroTime)*100) / focus_time_msec;  // Show result in %
      text += F("%");
    } else if (podomoroTime < ((focus_time_msec) + (break_time_msec))) {
      text += F("BREAK: ");
      text += ((podomoroTime - focus_time_msec) * 100) / break_time_msec;  // Show result in %
      text += F("%");
    }
  }
  CbOledDisplay.clearBuffer();
  CbOledDisplay.drawText(text.c_str(), 'C', 'M');
  CbOledDisplay.sendBuffer();
}


void buttonCb(uint16_t durationMs) {
  DBF("buttonCb() - durationMs=%d\n", durationMs);
  if (durationMs > 1000) {
    if (timerRunning) {
      // Shut down everything - STOP!
      timerRunning = false;
      timerPaused = false;
      CbLeds.clear();
      CbLeds.show();
    } else {
      timerRunning = true;
      timerPaused = false;
      podomoroTime = 0;
      updateDisplayTime = millis();
    }
  } else if (durationMs > 200) {
    if (timerRunning && !timerPaused) {
      timerPaused = true;
    } else if (!timerRunning) {
      timerRunning = true;
      timerPaused = false;
      podomoroTime = 0;
      updateDisplayTime = millis();
    } else {
      timerPaused = false;
    }
  }
}

void rotaryCb(int8_t diff, int32_t value) {
  DBF("rotaryCb diff=%d value=%d\n", diff, value);
  RotaryValue = value;
  display();
}

void setLEDs(uint32_t c) {
  for (int i = 0; i < 6; i++) {
    CbLeds.setPixelColor(i, c);
  }
  CbLeds.show();
}
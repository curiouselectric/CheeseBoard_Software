#include <Arduino.h>

// Include all dependent libraries so the Arduino IDE knows to add 
// the library paths into the build. Make sure all these libraries
// are installed on your system
#include <U8g2lib.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_NeoPixel.h>
#include <Encoder.h>
#include <ESP8266WiFi.h>

// Some includes from the Mutila library which is used by the 
// CheeseBoard library and others which I use 
#include <MutilaDebug.h>
#include <Millis.h>

// Include CheeseBoard headers for the components we'll be using in
// this example
#include <CbLeds.h>
#include <CbOled.h>
#include <CbRotaryInput.h>

// Include files from this example
#include "KnightRiderEffect.h"

bool twisted = false;

void rotaryCb(int8_t diff, int32_t value)
{
    DBF("rotaryCb diff=%d value=%d\n", diff, value);
    twisted = true;
    if (CbRotaryInput.buttonPushed()) {
        // If we push and hold while twisting, adjust the brightness
        if (diff > 0) {
            KnightRiderEffect.brighter();
        } else {
            KnightRiderEffect.dimmer();
        }
    } else {
        // Otherwise, adjust the frequency of the effect
        if (diff > 0) {
            KnightRiderEffect.setFrequency(KnightRiderEffect.getFrequency() * 1.15);
        } else {
            KnightRiderEffect.setFrequency(KnightRiderEffect.getFrequency() / 1.15);
        }
    }
}

void buttonCb(uint16_t durationMs)
{
    DBF("buttonCb() - durationMs=%d ", durationMs);
    if (!twisted) {
        KnightRiderEffect.changeColor();
        DBLN("changing color");
    }
    twisted = false;
}

void setup()
{
    Serial.begin(115200);
    delay(100);
    DBLN(F("S:setup"));

    // Init CbLeds
    CbLeds.begin();

    // Init the RotaryInput object
    CbRotaryInput.begin(buttonCb, rotaryCb);

    // Start initialize the KnightRiderEffect
    KnightRiderEffect.begin();

    CbOled.begin();
    CbOled.clearBuffer();
    CbOled.drawText("Twist: speed\nPush: color\nPush+twist: bright", 'L', 'M');
    CbOled.sendBuffer();

    DBLN(F("E:setup"));
}

void loop()
{
    KnightRiderEffect.update();
    CbRotaryInput.update();
}


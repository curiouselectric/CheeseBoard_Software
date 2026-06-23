#include <stdio.h>
#include <ESP8266WiFi.h>
#include "EspID.h"

EspID_ EspID;

void EspID_::begin()
{
    byte mac[6];
    WiFi.macAddress(mac);
    snprintf(_id, ESPID_LENGTH, "%02X%02X%02X", mac[3], mac[4], mac[5]);
}


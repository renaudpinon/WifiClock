/**
 * (c) Renaud PINON 2022.
 * 
 * Ce projet appartient à Renaud PINON. Le code peut être utilisé sans restriction à condition que l'appartenance du projet soit conservée et clairement indiquée.
 * */

#ifndef FUNC_SETUP_H
#define FUNC_SETUP_H

#include <Arduino.h>
#include <EEPROM.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include "defines.h"
#include "globals.h"
#include "structs.h"

void SetupEeprom()
{
    Serial.println("Initializing EEPROM...");

    Serial.println("Size: " + String(sizeof(_settings)));

    EEPROM.begin(sizeof(_settings));

    Serial.println("Reading EEPROM...");

    bool success = EepromReadSettings();

    if (success == false)
    {
        Serial.println("Error while reading settings from Eeprom.");
    }
    else
    {
        Serial.println("Settings read from EEPROM with success.");

        Serial.println("Login: " + String((const char*)_settings.HttpLogin) + String(" (longueur: ") + String(strlen(_settings.HttpLogin)) + String(")."));
    }
}

void SetupLed()
{
    pinMode(kPinPhotoResistor, INPUT);
    
    _luminosityFactor = LedGetLuminosityFactor();

    _pixelHours.begin();
    _pixelMinutes.begin();
    _pixelSeparator.begin();
}

void SetupSPIFFS()
{
    bool success = SPIFFS.begin(true);

    if(success == false)
    {
        Serial.println("An Error has occurred while mounting SPIFFS");
    }
}

void SetupStateSwitch()
{
    pinMode(kPinStateSwitch, INPUT);
}

void SetupTime()
{
    setTime(0);
}

#endif

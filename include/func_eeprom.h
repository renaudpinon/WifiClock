#ifndef FUNC_EEPROM_H
#define FUNC_EEPROM_H

#include <Arduino.h>
#include <EEPROM.h>
#include <time.h>
#include <Timezone.h>

#include "globals.h"
//#include "structs.h"

void EepromCreateDefaults()
{
    // Header:
    _settings.Header[0] = 'R';
    _settings.Header[1] = 'P';

    // Network:
    strcpy(_settings.WifiSSID, "");
    strcpy(_settings.WifiPassword, "");
    strcpy(_settings.HttpLogin, _default_http_username);
    strcpy(_settings.HttpPassword, _default_http_password);
    strcpy(_settings.TimeServer, "time.google.com");

    // Timezone and daylight saving:
    Serial.println("Settings Timezone...");
    TimeChangeRule STD = {"", First, Sun, Mar, 2, 0};
    _settings.StandardChangeRule = STD; // GMT +0 (UTC) by default.
    _settings.DaylightChangeRule = STD; // GMT +0 (UTC) by default.
    _settings.IsDaylightTimeSaving = false;
    _settings.SelectedTimezoneId = 5;

    // Color:
    _settings.Red = 0;
    _settings.Green = 0;
    _settings.Blue = 255;

    // Luminosity control:
    _settings.IsLuminosityEnabled = true;
    _settings.LuminosityMin = 0;          // Luminosity = 0 to 4096.
    _settings.LuminosityMax = 100;

    EepromSaveSettings();
}

bool EepromReadSettings()
{
    bool ret = false;

    int nRead = EEPROM.readBytes(0, &_settings, sizeof(_settings));

    Serial.println("Bytes read: " + String(nRead));

    if (nRead != sizeof(_settings))
    {
        Serial.println("Error while reading settings from EEPROM.");
    }
    else
    {
        Serial.println("Header0 = " + String(_settings.Header[0]));

#ifdef CLK_RESET_EEPROM
        Serial.println("Default settings creation...");
            // Init eeprom with default data:
        EepromCreateDefaults();
#else
        if (_settings.Header[0] != 'R' || _settings.Header[1] != 'P')
        {
            Serial.println("Default settings creation...");
            // Init eeprom with default data:
            EepromCreateDefaults();
        }
#endif

        ret = true;
    }

    return ret;
}

bool EepromSaveSettings()
{
    bool ret = false;

    Serial.println("Saving settings...");
    int nWritten = EEPROM.writeBytes(0, &_settings, sizeof(_settings));

    ret = (nWritten == sizeof(_settings));

    Serial.println(ret == true ? "Saving OK :)" : "Saving KO :(");
    if (ret == true)
    {
        ret = EEPROM.commit();
    }
    
    return ret;
}

#endif

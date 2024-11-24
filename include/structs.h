/**
 * (c) Renaud PINON 2022.
 * 
 * Ce projet appartient à Renaud PINON. Le code peut être utilisé sans restriction à condition que l'appartenance du projet soit conservée et clairement indiquée.
 * */

#ifndef STRUCTS_H
#define STRUCTS_H

#include <Arduino.h>
#include <Timezone.h>


typedef struct {
    char Header[2];

    char WifiSSID[33];
    char WifiPassword[65];
    char HttpLogin[26];
    char HttpPassword[26];

    char TimeServer[128];
    uint8_t SelectedTimezoneId;     // 255 if custom.
    bool IsDaylightTimeSaving;
    TimeChangeRule StandardChangeRule;
    TimeChangeRule DaylightChangeRule;

    uint8_t Red;
    uint8_t Green;
    uint8_t Blue;

    bool IsLuminosityEnabled;
    uint16_t LuminosityMin;
    uint16_t LuminosityMax;
} RPSettings;

#endif
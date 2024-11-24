/**
 * (c) Renaud PINON 2022.
 * 
 * Ce projet appartient à Renaud PINON. Le code peut être utilisé sans restriction à condition que l'appartenance du projet soit conservée et clairement indiquée.
 * */

#ifndef PROTOTYPES_H
#define PROTOTYPES_H

#include "enums.h"

// EEPROM functions:
void EepromCreateDefaults();
bool EepromReadSettings();
bool EepromSaveSettings();

// Led functions:
void LedPrint(String currentTime);
void LedPrintDigit(uint8_t digit, int digitPosition, Adafruit_NeoPixel* neoPixel, uint8_t r, uint8_t g, uint8_t b, bool rightToLeft);
void LedPrintSeparator(uint8_t r, uint8_t g, uint8_t b);
float LedGetLuminosityFactor();

// Setup Functions:
void SetupEeprom();
void SetupLed();
void SetupSPIFFS();
void SetupState();
void SetupTime();

// Switch state functions:
SwitchStateType SwitchStateGet();

// Time functions:
void TimeInitNTP();
String TimeGet();
void TimeSetWithNTP();
unsigned long TimeGetTimestamp();

// Wifi functions:
void WifiChangeMode(SwitchStateType state);
bool WifiIsConnected();
void WifiHttpRegisterGetFile(String fileUrl);
void WifiHttpRegisterGetFile(String fileUrl, AwsTemplateProcessor templateProcessor);
void WifiHttpRegisterPostAction(String actionUrl);
void WifiHttpRegisterPostActionColorsTab(AsyncWebServerRequest* request);
void WifiHttpRegisterPostActionNetworkTab(AsyncWebServerRequest* request);
void WifiHttpRegisterPostActionTimeTab(AsyncWebServerRequest* request);
String WifiHttpProcessor(const String& var);
IPAddress WifiSetAPMode();
IPAddress WifiSetSSIDMode();
bool WifiStop();

#endif

#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>

#include <Adafruit_NeoPixel.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <NTPClient.h>
#include <time.h>
#include <Timezone.h>

#include "defines.h"
#include "enums.h"
#include "structs.h"

#include "prototypes.h"

//const char* ssid = "Lvb-810213";
//const char* password = "wifi8944";
const char* _defaultSSID = CLK_NAME;
const char* _defaultPassword = "clock_1234";

const char* _default_http_username = "admin";
const char* _default_http_password = "admin";

AsyncWebServer _server(80);

SwitchStateType _state = SwitchStateTypeNone;
RPSettings _settings;

unsigned long _currentTimestamp = 0;
unsigned long _timeSwitchState = 0;
unsigned long _timeNTPRequest = 0;


WiFiUDP _ntpUDP;  // WiFiUdp lib initialization.
NTPClient _timeClient(_ntpUDP); // NTP Client initialization.

char _lastDisplayedTime[6] = "99:99";

// Leds:
Adafruit_NeoPixel _pixelHours = Adafruit_NeoPixel(kNumPixels, kPinNeopixelHours, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel _pixelSeparator = Adafruit_NeoPixel(2, kPinNeopixelSeparator, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel _pixelMinutes = Adafruit_NeoPixel(kNumPixels, kPinNeopixelMinutes, NEO_GRB + NEO_KHZ800);

float _luminosityFactor = 1.0;

const bool _digitsLTR[10][7] = {
                      { true, true, true, false, true, true, true },      // 0.
                      { false, false, true, false, false, false, true },      // 1.
                      { true, true, false, true, false, true, true },      // 2.
                      { false, true, true, true, false, true, true },      // 3.
                      { false, false, true, true, true, false, true },      // 4.
                      { false, true, true, true, true, true, false },      // 5.
                      { true, true, true, true, true, true, false },      // 6.
                      { false, false, true, false, false, true, true },      // 7.
                      { true, true, true, true, true, true, true },      // 8.
                      { false, true, true, true, true, true, true },      // 9.
                      };
                      
const bool _digitsRTL[10][7] = {
                      { true, true, true, false, true, true, true },      // 0.
                      { true, false, false, false, true, false, false },      // 1.
                      { false, true, true, true, true, true, false },      // 2.
                      { true, true, false, true, true, true, false },      // 3.
                      { true, false, false, true, true, false, true },      // 4.
                      { true, true, false, true, false, true, true },      // 5.
                      { true, true, true, true, false, true, true },      // 6.
                      { true, false, false, false, true, true, false },      // 7.
                      { true, true, true, true, true, true, true },      // 8.
                      { true, true, false, true, true, true, true },      // 9.
                      };

bool _settingsChanged = false;


#endif
#include <Arduino.h>

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#define CLK_NAME "Clk-01-002"
//#define CLK_RESET_EEPROM  // uncomment to reset EEPROM settings.

#define __SPIFFS_Last_Modified__ "Sun, 14 Aug 2022 16:00:00 GMT"

#include "enums.h"
#include "globals.h"

#include "func_eeprom.h"
#include "func_led.h"
#include "func_setup.h"
#include "func_switch_state.h"
#include "func_time.h"
#include "func_wifi.h"

void setup() 
{
	Serial.begin(115200);

	delay(800);

	SetupEeprom();

	SetupLed();

	SetupStateSwitch();

	SetupSPIFFS();

	TimeInitNTP();

	TimeSetWithNTP(); // set time to 0;
}

void loop() 
{
	unsigned long now = millis();
	

	// Monitoring witch state changes:
	if (_state == SwitchStateTypeNone || now - _timeSwitchState > kDelaySwitchState || _timeSwitchState > now)
	{
		//Serial.println("Getting state...");
		SwitchStateType newState = SwitchStateGet();
		_timeSwitchState = now;

		// SwitchState has been activated:
		if (newState != _state)
		{
			Serial.println("State changed !");
			_state = newState;

			WifiChangeMode(_state);
		}
	}

	if (WifiIsConnected() == true)
	{
		if (_timeNTPRequest == 0 || now - _timeNTPRequest > kDelayNTPRequest || _timeNTPRequest > now)
		{
			TimeSetWithNTP();
			_timeNTPRequest = now;
		}

		String currentTime = TimeGet();
		float newLuminosity = LedGetLuminosityFactor();
		float lumDelta = (_settings.IsLuminosityEnabled == false) ? 0.0 : abs(_luminosityFactor - newLuminosity);
		
		if (currentTime.compareTo(_lastDisplayedTime) != 0 || lumDelta > 0.02 || _settingsChanged == true)
		{
			_luminosityFactor = newLuminosity;
			Serial.println("Printing " + currentTime + String(", lum: ") + String(_luminosityFactor) +".");
			LedPrint(currentTime);
			strcpy(_lastDisplayedTime, currentTime.c_str());
			
			_settingsChanged = false;
		}
	}

	delay(500);
}

#ifndef FUNC_TIME_H
#define FUNC_TIME_H

#include <Arduino.h>
#include <time.h>
#include <Timezone.h>

#include "globals.h"

String TimeGet()
{
	time_t utc = now();
	TimeChangeRule* unusedChangedRule;

	Timezone tz = (_settings.IsDaylightTimeSaving == false) ? Timezone(_settings.StandardChangeRule) : Timezone(_settings.StandardChangeRule, _settings.DaylightChangeRule);
	time_t local = tz.toLocal(utc, &unusedChangedRule);
	
	// Hours:
	return String( String((hour(local) < 10) ? "0" : "" ) + String(hour(local)) + ":" + String((minute(local) < 10) ? "0" : "" ) + String(minute(local)) );
}

unsigned long TimeGetTimestamp()
{
	unsigned long ret = 0;

	if (WifiIsConnected() == false)
	{
		Serial.println("TimeGetTimestamp(): not connected");
		ret = millis() / 1000;
	}
	else
	{
		_timeClient.update();   // NTP server request.
		ret = _timeClient.getEpochTime(); // UNIX timestamp.
		Serial.println("TimeGetTimestamp(): connected ! Timestamp = " + String(ret));
	}

	return ret;
}

void TimeInitNTP()
{
	_timeClient.end(); // TODO: à tester.

	Serial.println("Timeserver = " + String(_settings.TimeServer));
	_timeClient = NTPClient(_ntpUDP, _settings.TimeServer);
}

void TimeSetWithNTP()
{
	unsigned long timestamp = TimeGetTimestamp();
	setTime(timestamp);
}



#endif
#ifndef FUNC_LED_H
#define FUNC_LED_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#include "globals.h"

float LedGetLuminosityFactor()
{
	float ret = 1.0;

	uint16_t lumValue = analogRead(kPinPhotoResistor);;
	lumValue = lumValue > 4096 ? 4096 : lumValue;
	//Serial.println("lumValue: " + String(lumValue));

	// Borned to 0.1 => 1.0:
	ret = (float)(((float)_settings.LuminosityMin + ((float)lumValue  * (float)(_settings.LuminosityMax - _settings.LuminosityMin) / 4096.0)) / 100.0);

	//Serial.println("_luminosityFactor: " + String(_luminosityFactor));

	return ret;
}

void LedPrint(String currentTime)
{
	if (currentTime.length() == 5)
	{
		int r = _settings.Red;
		int g = _settings.Green;
		int b = _settings.Blue;

		if (_settings.IsLuminosityEnabled == true)
		{
			r = (uint8_t)((float)r * _luminosityFactor);
			g = (uint8_t)((float)g * _luminosityFactor);
			b = (uint8_t)((float)b * _luminosityFactor);
		}

		/*Serial.println("LED red: " + String(r));
		Serial.println("LED green: " + String(g));
		Serial.println("LED blue: " + String(b));*/

		// Hours:
		LedPrintDigit(String(currentTime.charAt(0)).toInt(), 1, &_pixelHours, r, g, b, true);
		LedPrintDigit(String(currentTime.charAt(1)).toInt(), 0, &_pixelHours, r, g, b, true);

		// Separator:
		LedPrintSeparator(r, g, b);

		// Minutes:
		LedPrintDigit(String(currentTime.charAt(3)).toInt(), 1, &_pixelMinutes, r, g, b, false);
		LedPrintDigit(String(currentTime.charAt(4)).toInt(), 0, &_pixelMinutes, r, g, b, false);
	}
}

void LedPrintDigit(uint8_t digit, int digitPosition, Adafruit_NeoPixel* neoPixel, uint8_t r, uint8_t g, uint8_t b, bool rightToLeft)
{
  if (neoPixel != NULL)
  {
    int start = 0;
    const bool *indices =  rightToLeft == true ? _digitsRTL[digit] : _digitsLTR[digit];

	/*if (_settings.IsLuminosityEnabled == true)
	{
		r = (uint8_t)((float)r * _luminosityFactor);
		g = (uint8_t)((float)g * _luminosityFactor);
		b = (uint8_t)((float)b * _luminosityFactor);
	}

	Serial.println("LED red: " + String(r));
	Serial.println("LED green: " + String(g));
	Serial.println("LED blue: " + String(b));*/
    
    if ((digitPosition == 1 && rightToLeft == true) || (digitPosition == 0 && rightToLeft == false))
    {
      start = 7;
    }
        
    for (int i = 0 ; i < 7 ; i++)
    {
      neoPixel->setPixelColor(i + start, *(indices + i) == 0 ? neoPixel->Color(0,0,0) : neoPixel->Color(r, g, b));
    }

	neoPixel->show();
    
  }
}

void LedPrintSeparator(uint8_t r, uint8_t g, uint8_t b)
{
	/*if (_settings.IsLuminosityEnabled == true)
	{
		r = (uint8_t)((float)r * _luminosityFactor);
		g = (uint8_t)((float)g * _luminosityFactor);
		b = (uint8_t)((float)b * _luminosityFactor);
	}*/

	_pixelSeparator.setPixelColor(0, Adafruit_NeoPixel::Color(r, g, b));
	_pixelSeparator.setPixelColor(1, Adafruit_NeoPixel::Color(r, g, b));
	_pixelSeparator.show();
}

#endif
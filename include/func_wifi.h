/**
 * (c) Renaud PINON 2022.
 * 
 * Ce projet appartient à Renaud PINON. Le code peut être utilisé sans restriction à condition que l'appartenance du projet soit conservée et clairement indiquée.
 * */

#ifndef FUNC_WIFI_H
#define FUNC_WIFI_H

#include <Arduino.h>
#include <EEPROM.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>

//#include "esp_wifi.h"


#include "globals.h"

void WifiChangeMode(SwitchStateType state)
{
    IPAddress ip;
    bool success = WifiStop();

    Serial.println("WiFiStop: " + String(success == true ? "OK :)" : "KO :("));

    if (success == true)
    {
        switch (state)
        {
            case SwitchStateTypeAPMode:
                ip = WifiSetAPMode();
                break;
            
            case SwitchStateTypeWifiMode:
            default:
                ip = WifiSetSSIDMode();
                break;
        }
    }

    if (ip != IPAddress())
    {
        Serial.println("Ip address found.");
        //esp_wifi_set_ps (WIFI_PS_NONE);

        _server.serveStatic("/css/all.css", SPIFFS, "/css/all.css").setLastModified(__SPIFFS_Last_Modified__);
        _server.serveStatic("/js/all.min.js", SPIFFS, "/js/all.min.js").setLastModified(__SPIFFS_Last_Modified__);

        _server.serveStatic("/img/wifi.png", SPIFFS, "/img/wifi.png").setLastModified(__SPIFFS_Last_Modified__);
        _server.serveStatic("/img/settings.png", SPIFFS, "/img/settings.png").setLastModified(__SPIFFS_Last_Modified__);

        WifiHttpRegisterGetFile("/", WifiHttpProcessor);
        WifiHttpRegisterGetFile("/index.html", WifiHttpProcessor);

        WifiHttpRegisterPostAction("/action.html");

        // Start the http server:
        _server.begin();
    }
}

bool WifiIsConnected()
{
    return WiFi.getMode() == WIFI_MODE_STA && WiFi.isConnected() == true;
}

void WifiHttpRegisterGetFile(String fileUrl)
{
    WifiHttpRegisterGetFile(fileUrl, nullptr);
}

void WifiHttpRegisterGetFile(String fileUrl, AwsTemplateProcessor templateProcessor)
{
    _server.on(fileUrl.c_str(), HTTP_GET, [fileUrl, templateProcessor](AsyncWebServerRequest *request)
    {
        if(request->authenticate(_settings.HttpLogin, _settings.HttpPassword) == false)
        {
            return request->requestAuthentication();
        }
        else
        {
            String tmpFileUrl = String(fileUrl);
            if (tmpFileUrl.compareTo("/") == 0)
            {
                tmpFileUrl = String("/index.html");
            }

            if (templateProcessor == nullptr)
            {
                request->send(SPIFFS, tmpFileUrl, String());
            }
            else
            {
                request->send(SPIFFS, tmpFileUrl, String(), false, templateProcessor);
            }
        }
    });
}

void WifiHttpRegisterPostAction(String actionUrl)
{
    _server.on(actionUrl.c_str(), HTTP_POST, [actionUrl](AsyncWebServerRequest* request)
    {
        Serial.println("WifiHttpRegisterPostAction...");
        if(request->authenticate(_settings.HttpLogin, _settings.HttpPassword) == false)
        {
            return request->requestAuthentication();
        }
        else
        {
            // Time Tab:
            WifiHttpRegisterPostActionTimeTab(request);

            // Colors Tab:
            WifiHttpRegisterPostActionColorsTab(request);

            // Network Tab:
            WifiHttpRegisterPostActionNetworkTab(request);

            // Saving settings:
            request->redirect(EepromSaveSettings() == true ? F("/index.html?update=success") : F("/index.html?update=failure"));
        }
    });
}

void WifiHttpRegisterPostActionColorsTab(AsyncWebServerRequest* request)
{
    AsyncWebParameter* red = (request->hasParam(F("Settings_Red"), true) == false) ? nullptr : request->getParam(F("Settings_Red"), true);
    AsyncWebParameter* green = (request->hasParam(F("Settings_Green"), true) == false) ? nullptr : request->getParam(F("Settings_Green"), true);
    AsyncWebParameter* blue = (request->hasParam(F("Settings_Blue"), true) == false) ? nullptr : request->getParam(F("Settings_Blue"), true);
    AsyncWebParameter* luminosity = (request->hasParam(F("Settings_Luminosity"), true) == false) ? nullptr : request->getParam(F("Settings_Luminosity"), true);
    AsyncWebParameter* isLuminosityEnabled = (request->hasParam(F("Settings_IsLuminosityEnabled"), true) == false) ? nullptr : request->getParam(F("Settings_IsLuminosityEnabled"), true);

    _settings.Red = red == nullptr ? _settings.Red : (uint8_t)red->value().toInt();
    _settings.Green = green == nullptr ? _settings.Green : (uint8_t)green->value().toInt();
    _settings.Blue = blue == nullptr ? _settings.Blue : (uint8_t)blue->value().toInt();
    if (luminosity != nullptr && luminosity->value().indexOf(",") > -1)
    {
        int index = luminosity->value().indexOf(",");
        _settings.LuminosityMin = luminosity->value().substring(0, index).toInt();
        _settings.LuminosityMax = luminosity->value().substring(index + 1).toInt();
    }

    /*Serial.println("red: " + (red != nullptr ? red->value() : "??"));
    Serial.println("green: " + (green != nullptr ? green->value() : "??"));
    Serial.println("blue: " + (blue != nullptr ? blue->value() : "??"));*/
    
    _settings.IsLuminosityEnabled = (isLuminosityEnabled != nullptr);

    _settingsChanged = true;
}

void WifiHttpRegisterPostActionNetworkTab(AsyncWebServerRequest* request)
{
    bool shouldResetConnection = false;

    AsyncWebParameter* wifiSSID = (request->hasParam(F("Settings_WifiSSID"), true) == false) ? nullptr : request->getParam(F("Settings_WifiSSID"), true);
    AsyncWebParameter* wifiPassword = (request->hasParam(F("Settings_WifiPassword"), true) == false) ? nullptr : request->getParam(F("Settings_WifiPassword"), true);
    AsyncWebParameter* httpLogin = (request->hasParam(F("Settings_HttpLogin"), true) == false) ? nullptr : request->getParam(F("Settings_HttpLogin"), true);
    AsyncWebParameter* httpPassword = (request->hasParam(F("Settings_HttpPassword"), true) == false) ? nullptr : request->getParam(F("Settings_HttpPassword"), true);
    
    // Wifi login and password:
    if (wifiSSID != nullptr && wifiSSID->value().compareTo(_settings.WifiSSID) != 0)
    {
        strcpy(_settings.WifiSSID, wifiSSID->value().c_str());

        shouldResetConnection = true;
    }

    if (wifiPassword != nullptr && wifiPassword->value().compareTo("") != 0)
    {
        strcpy(_settings.WifiPassword, wifiPassword->value().c_str());

        shouldResetConnection = true;
    }
    
    // Http login and password.
    if (httpLogin != nullptr) strcpy(_settings.HttpLogin, httpLogin->value().c_str());

    if (httpPassword != nullptr && httpPassword->value().compareTo("") != 0)
    {
        strcpy(_settings.HttpPassword, httpPassword->value().c_str());
    }

    // TODO: reset connection if shouldResetConnection is true.
}

void WifiHttpRegisterPostActionTimeTab(AsyncWebServerRequest* request)
{
    AsyncWebParameter* timeServer = (request->hasParam(F("Settings_TimeServer"), true) == false) ? nullptr : request->getParam(F("Settings_TimeServer"), true);
    AsyncWebParameter* timeZoneType = (request->hasParam(F("grpTimezoneType"), true) == false) ? nullptr : request->getParam(F("grpTimezoneType"), true);
    
    AsyncWebParameter* selectedTimezoneId = (request->hasParam(F("Settings_SelectedTimezoneId"), true) == false) ? nullptr : request->getParam(F("Settings_SelectedTimezoneId"), true);
    AsyncWebParameter* isDaylightTimeSaving = (request->hasParam(F("Settings_IsDaylightTimeSaving"), true) == false) ? nullptr : request->getParam(F("Settings_IsDaylightTimeSaving"), true);
    AsyncWebParameter* stdMonth = (request->hasParam(F("Settings_STD_Month"), true) == false) ? nullptr : request->getParam(F("Settings_STD_Month"), true);
    AsyncWebParameter* sdlMonth = (request->hasParam(F("Settings_SDL_Month"), true) == false) ? nullptr : request->getParam(F("Settings_SDL_Month"), true);
    AsyncWebParameter* stdWeek = (request->hasParam(F("Settings_STD_Week"), true) == false) ? nullptr : request->getParam(F("Settings_STD_Week"), true);
    AsyncWebParameter* sdlWeek = (request->hasParam(F("Settings_SDL_Week"), true) == false) ? nullptr : request->getParam(F("Settings_SDL_Week"), true);
    AsyncWebParameter* stdDayOfWeek = (request->hasParam(F("Settings_STD_DayOfWeek"), true) == false) ? nullptr : request->getParam(F("Settings_STD_DayOfWeek"), true);
    AsyncWebParameter* sdlDayOfWeek = (request->hasParam(F("Settings_SDL_DayOfWeek"), true) == false) ? nullptr : request->getParam(F("Settings_SDL_DayOfWeek"), true);
    AsyncWebParameter* stdHour = (request->hasParam(F("Settings_STD_Hour"), true) == false) ? nullptr : request->getParam(F("Settings_STD_Hour"), true);
    AsyncWebParameter* sdlHour = (request->hasParam(F("Settings_SDL_Hour"), true) == false) ? nullptr : request->getParam(F("Settings_SDL_Hour"), true);
    AsyncWebParameter* stdOffset = (request->hasParam(F("Settings_STD_Offset"), true) == false) ? nullptr : request->getParam(F("Settings_STD_Offset"), true);
    AsyncWebParameter* sdlOffset = (request->hasParam(F("Settings_SDL_Offset"), true) == false) ? nullptr : request->getParam(F("Settings_SDL_Offset"), true);

    if (timeServer != nullptr) 
    {
        strcpy(_settings.TimeServer, timeServer->value().c_str());
        TimeInitNTP();
    }

    if (timeZoneType != nullptr && timeZoneType->value().toInt() == 1)
    {
        _settings.SelectedTimezoneId = 255;  
    }
    else
    {
        _settings.SelectedTimezoneId = (selectedTimezoneId == nullptr) ? _settings.SelectedTimezoneId : (uint8_t)selectedTimezoneId->value().toInt();
    }
    
    _settings.IsDaylightTimeSaving = (isDaylightTimeSaving != nullptr);

    TimeChangeRule standardChangeRule = {
        "STD",
        (stdWeek == nullptr) ? _settings.StandardChangeRule.week : (uint8_t)stdWeek->value().toInt(),
        (stdDayOfWeek == nullptr) ? _settings.StandardChangeRule.dow  : (uint8_t)stdDayOfWeek->value().toInt(),
        (stdMonth == nullptr) ? _settings.StandardChangeRule.month  : (uint8_t)stdMonth->value().toInt(),
        (stdHour == nullptr) ? _settings.StandardChangeRule.hour  : (uint8_t)stdHour->value().toInt(),
        (stdOffset == nullptr) ? _settings.StandardChangeRule.offset  : (int)stdOffset->value().toInt()
    };

    TimeChangeRule daylightChangeRule = {
        "SDL",
        (sdlWeek == nullptr) ? _settings.DaylightChangeRule.week : (uint8_t)sdlWeek->value().toInt(),
        (sdlDayOfWeek == nullptr) ? _settings.DaylightChangeRule.dow  : (uint8_t)sdlDayOfWeek->value().toInt(),
        (sdlMonth == nullptr) ? _settings.DaylightChangeRule.month  : (uint8_t)sdlMonth->value().toInt(),
        (sdlHour == nullptr) ? _settings.DaylightChangeRule.hour  : (uint8_t)sdlHour->value().toInt(),
        (sdlOffset == nullptr) ? _settings.DaylightChangeRule.offset  : (int)sdlOffset->value().toInt()
    };

    _settings.StandardChangeRule = standardChangeRule;

    if (isDaylightTimeSaving != nullptr)
    {
        _settings.DaylightChangeRule = daylightChangeRule;
    }
    else
    {
        TimeChangeRule nullChangeRule = { "SDL", First, Sun, 1, 0, 0};
        _settings.DaylightChangeRule = nullChangeRule;
    }
}

String WifiHttpProcessor(const String& var)
{
    String ret = String();

    // Time Tab:
    if (var == "Settings_TimeServer")
    {
        ret = String(_settings.TimeServer);
    }
    else if (var == "Settings_SelectedTimezoneId")
    {
        ret = String(_settings.SelectedTimezoneId);
    }
    else if (var == "Settings_IsDaylightTimeSaving")
    {
        ret = String(_settings.IsDaylightTimeSaving == true ? "true" : "false");
    }
    else if (var == "Settings_STD_Month")
    {
        ret = String(_settings.StandardChangeRule.month);
    }
    else if (var == "Settings_SDL_Month")
    {
        ret = String(_settings.DaylightChangeRule.month);
    }
    else if (var == "Settings_STD_Week")
    {
        ret = String(_settings.StandardChangeRule.week);
    }
    else if (var == "Settings_SDL_Week")
    {
        ret = String(_settings.DaylightChangeRule.week);
    }
    else if (var == "Settings_STD_DayOfWeek")
    {
        ret = String(_settings.StandardChangeRule.dow);
    }
    else if (var == "Settings_SDL_DayOfWeek")
    {
        ret = String(_settings.DaylightChangeRule.dow);
    }
    else if (var == "Settings_STD_Hour")
    {
        ret = String(_settings.StandardChangeRule.hour);
    }
    else if (var == "Settings_SDL_Hour")
    {
        ret = String(_settings.DaylightChangeRule.hour);
    }
    else if (var == "Settings_STD_Offset")
    {
        ret = String(_settings.StandardChangeRule.offset);
    }
    else if (var == "Settings_SDL_Offset")
    {
        ret = String(_settings.DaylightChangeRule.offset);
    }
    // Colors Tab:
    else if (var == "Settings_Red")
    {
        ret = String(_settings.Red);
    }
    else if (var == "Settings_Green")
    {
        ret = String(_settings.Green);
    }
    else if (var == "Settings_Blue")
    {
        ret = String(_settings.Blue);
    }
    else if (var == "Settings_IsLuminosityEnabled")
    {
        ret = String(_settings.IsLuminosityEnabled == true ? "true" : "false");
    }
    else if (var == "Settings_LuminosityMin")
    {
        ret = String(_settings.LuminosityMin);
    }
    else if (var == "Settings_LuminosityMax")
    {
        ret = String(_settings.LuminosityMax);
    }
    // Network Tab:
    else if (var == "Settings_WifiSSID")
    {
        ret = String(_settings.WifiSSID);
    }
    else if (var == "Settings_HttpLogin")
    {
        ret = String(_settings.HttpLogin);
    }

    return ret;
}

IPAddress WifiSetAPMode()
{
    IPAddress ret(0, 0, 0, 0);

    Serial.print("Setting AP (Access Point)…");

    WiFi.softAP(_defaultSSID, _defaultPassword);

    ret = WiFi.softAPIP();

    Serial.println("AP IP address: " + ret.toString());

    return ret;
}

IPAddress WifiSetSSIDMode()
{
    IPAddress ret(0, 0, 0, 0);

    if (strlen(_settings.WifiSSID) == 0)
    {
        Serial.println("No wifi ssid specified.");
    }
    else
    {
        int counter = 0;

        WiFi.begin(_settings.WifiSSID, _settings.WifiPassword);
        while (WiFi.status() != WL_CONNECTED && counter < 35) 
        {
            Serial.println("Connecting to WiFi..");
            delay(1000);
            counter++;
        }

        if (WiFi.status() != WL_CONNECTED)
        {
            Serial.println("Cannot connect to SSID " + String(_settings.WifiSSID) + ".");
        }
        else
        {
            ret = WiFi.localIP();

            // Print ESP Local IP Address
            Serial.println("Local IP: " + ret.toString()); 
        }
    }
    
    return ret;
}

bool WifiStop()
{
    bool ret = true;

    if (WiFi.isConnected() == true)
    {
        // Station mode:
        Serial.println("Disconnecting station mode...");
        ret = WiFi.disconnect();
    }
    else if (WiFi.softAPIP().toString().compareTo(IPAddress().toString()) != 0) // IP for Access Point mode is not empty.
    {
        // Access point mode:
        Serial.println("Disconnecting access point mode...");
        ret = WiFi.softAPdisconnect();
    }

    Serial.println("Wifi Disconnection: " + String(ret == true ? "OK." : "Error."));

    return ret;
}

#endif

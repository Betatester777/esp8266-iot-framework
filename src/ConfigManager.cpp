#include <EEPROM.h>
#include <Arduino.h>
#include <ConfigManager.h>
#include <WiFiManager.h>
#include "ArduinoJson.h"
#include "WebServer.h"

//class functions
bool ConfigManager::begin(int numBytes)
{
    EEPROM.begin(numBytes);

    bool returnValue = true;
    testConnectionResult = "{}";
    testConnection = false;

    uint32_t storedVersion;
    uint8_t scopeChecksum = 0;

    uint32_t startByte = 0;

    uint8_t resetCopes = 0;

    EEPROM.get(startByte, storedVersion);

    startByte = sizeof(storedVersion);
    Serial.println("Load [scope=LEGAL, start=" + String(startByte) + "]");
    EEPROM.get(startByte, legal);
    EEPROM.get(startByte + sizeof(legal), scopeChecksum);

    if (scopeChecksum != checksum(reinterpret_cast<uint8_t *>(&legal), sizeof(legal)) || storedVersion != configVersion)
    {
        Serial.println(PSTR("Checksum mismatch [scope=LEGAL]"));
        resetCopes |= SCOPE_LEGAL;
    }

    startByte += sizeof(legal) + sizeof(scopeChecksum);
    Serial.println("Load [scope=WIFI, start=" + String(startByte) + "]");
    EEPROM.get(startByte, wifi);
    EEPROM.get(startByte + sizeof(wifi), scopeChecksum);

    if (scopeChecksum != checksum(reinterpret_cast<uint8_t *>(&wifi), sizeof(wifi)) || storedVersion != configVersion)
    {
        Serial.println(PSTR("Checksum mismatch [scope=WIFI]"));
        resetCopes |= SCOPE_WIFI;
        returnValue = false;
    }

    startByte += sizeof(wifi) + sizeof(scopeChecksum);
    Serial.println("Load [scope=TIME, start=" + String(startByte) + "]");
    EEPROM.get(startByte, time);
    EEPROM.get(startByte + sizeof(time), scopeChecksum);

    if (scopeChecksum != checksum(reinterpret_cast<uint8_t *>(&time), sizeof(time)) || storedVersion != configVersion)
    {
        Serial.println(PSTR("Checksum mismatch [scope=TIME]"));
        resetCopes |= SCOPE_TIME;
        returnValue = false;
    }

    startByte += sizeof(time) + sizeof(scopeChecksum);
    Serial.println("Load [scope=SERVER, start=" + String(startByte) + "]");
    EEPROM.get(startByte, server);
    EEPROM.get(startByte + sizeof(server), scopeChecksum);

    if (scopeChecksum != checksum(reinterpret_cast<uint8_t *>(&server), sizeof(server)) || storedVersion != configVersion)
    {
        Serial.println(PSTR("Checksum mismatch [scope=SERVER]"));
        resetCopes |= SCOPE_SERVER;
        returnValue = false;
    }

    startByte += sizeof(server) + sizeof(scopeChecksum);
    Serial.println("Load [scope=SERVER_TEST, start=" + String(startByte) + "]");
    EEPROM.get(startByte, server_test);
    EEPROM.get(startByte + sizeof(server_test), scopeChecksum);

    if (scopeChecksum != checksum(reinterpret_cast<uint8_t *>(&server_test), sizeof(server_test)) || storedVersion != configVersion)
    {
        Serial.println(PSTR("Checksum mismatch [scope=SERVER_TEST]"));
        resetCopes |= SCOPE_SERVER_TEST;
        returnValue = false;
    }

    startByte += sizeof(server_test) + sizeof(scopeChecksum);
    Serial.println("Load [scope=TIMER, start=" + String(startByte) + "]");
    EEPROM.get(startByte, timer);
    EEPROM.get(startByte + sizeof(timer), scopeChecksum);

    if (scopeChecksum != checksum(reinterpret_cast<uint8_t *>(&timer), sizeof(timer)) || storedVersion != configVersion)
    {
        Serial.println(PSTR("Checksum mismatch [scope=TIMER]"));
        resetCopes |= SCOPE_TIMER;
        returnValue = false;
    }

    startByte += sizeof(timer) + sizeof(scopeChecksum);
    Serial.println("Load [scope=SETTINGS, start=" + String(startByte) + "]");
    EEPROM.get(startByte, settings);
    EEPROM.get(startByte + sizeof(settings), scopeChecksum);

    if (scopeChecksum != checksum(reinterpret_cast<uint8_t *>(&settings), sizeof(settings)) || storedVersion != configVersion)
    {
        Serial.println(PSTR("Checksum mismatch [scope=SETTINGS]"));
        resetCopes |= SCOPE_SETTINGS;
        returnValue = false;
    }

    Serial.println("Load EEPROM " + String(startByte + sizeof(settings) + sizeof(scopeChecksum)));

    if (resetCopes > 0)
    {
        reset(resetCopes);
    }

    return returnValue;
}

String ConfigManager::getDeviceName()
{
    String deviceId = String(PRODUCT_NAME) + "-";
    String macAddress = wifiManager.getMACAddress();
    Serial.println("MAC: " + macAddress);
    for (unsigned int i = macAddress.length() - 6; i < macAddress.length(); i++)
    {
        if (macAddress[i] != ':')
        {
            deviceId += macAddress[i];
        }
    }
    return deviceId;
}

void ConfigManager::reset(uint8_t scope)
{
    if ((scope & SCOPE_LEGAL) == SCOPE_LEGAL)
    {
        memcpy_P(&legal, &legalDefaults, sizeof(legal));
    }

    if ((scope & SCOPE_WIFI) == SCOPE_WIFI)
    {
        wifiManager.forget();
        memcpy_P(&wifi, &wifiDefaults, sizeof(wifi));
    }

    if ((scope & SCOPE_WIFI_TEST) == SCOPE_WIFI_TEST)
    {
        memcpy_P(&wifi_test, &wifiTestDefaults, sizeof(wifi_test));
    }

    if ((scope & SCOPE_TIME) == SCOPE_TIME)
    {
        memcpy_P(&time, &timeDefaults, sizeof(time));
    }

    if ((scope & SCOPE_SERVER) == SCOPE_SERVER)
    {
        memcpy_P(&server, &serverDefaults, sizeof(server));
    }

    if ((scope & SCOPE_SERVER_TEST) == SCOPE_SERVER_TEST)
    {
        memcpy_P(&server_test, &serverTestDefaults, sizeof(server_test));
    }

    if ((scope & SCOPE_TIMER) == SCOPE_TIMER)
    {
        memcpy_P(&timer, &timerDefaults, sizeof(timer));
    }

    if ((scope & SCOPE_SETTINGS) == SCOPE_SETTINGS)
    {
        memcpy_P(&settings, &settingsDefaults, sizeof(settings));
    }

    save(scope);
    GUI.publishStatus();
}

int ConfigManager::save(uint8_t scope)
{
    uint32_t startByte = 0;
    uint8_t scopeChecksum = 0;

    EEPROM.put(startByte, configVersion);

    startByte = sizeof(configVersion);

    if ((scope & SCOPE_LEGAL) == SCOPE_LEGAL)
    {
        scopeChecksum = checksum(reinterpret_cast<uint8_t *>(&legal), sizeof(legal));
        Serial.println("Save [scope=LEGAL, start=" + String(startByte) + "]");
        EEPROM.put(startByte, legal);
        EEPROM.put(startByte + sizeof(legal), scopeChecksum);
    }

    startByte += sizeof(legal) + sizeof(scopeChecksum);
    if ((scope & SCOPE_WIFI) == SCOPE_WIFI)
    {
        scopeChecksum = checksum(reinterpret_cast<uint8_t *>(&wifi), sizeof(wifi));
        Serial.println("Save [scope=WIFI, start=" + String(startByte) + "]");
        EEPROM.put(startByte, wifi);
        EEPROM.put(startByte + sizeof(wifi), scopeChecksum);
    }

    startByte += sizeof(wifi) + sizeof(scopeChecksum);
    if ((scope & SCOPE_WIFI_TEST) == SCOPE_WIFI_TEST)
    {
        scopeChecksum = checksum(reinterpret_cast<uint8_t *>(&wifi_test), sizeof(wifi_test));
        Serial.println("Save [scope=WIFI_TEST, start=" + String(startByte) + "]");
        EEPROM.put(startByte, wifi_test);
        EEPROM.put(startByte + sizeof(wifi_test), scopeChecksum);
    }

    startByte += sizeof(wifi_test) + sizeof(scopeChecksum);
    if ((scope & SCOPE_TIME) == SCOPE_TIME)
    {
        scopeChecksum = checksum(reinterpret_cast<uint8_t *>(&time), sizeof(time));
        Serial.println("Save [scope=TIME, start=" + String(startByte) + "]");
        EEPROM.put(startByte, time);
        EEPROM.put(startByte + sizeof(time), scopeChecksum);
    }

    startByte += sizeof(time) + sizeof(scopeChecksum);
    if ((scope & SCOPE_SERVER) == SCOPE_SERVER)
    {
        scopeChecksum = checksum(reinterpret_cast<uint8_t *>(&server), sizeof(server));
        Serial.println("Save [scope=SERVER, start=" + String(startByte) + "]");
        EEPROM.put(startByte, server);
        EEPROM.put(startByte + sizeof(server), scopeChecksum);
    }

    startByte += sizeof(server) + sizeof(scopeChecksum);
    if ((scope & SCOPE_SERVER_TEST) == SCOPE_SERVER_TEST)
    {
        scopeChecksum = checksum(reinterpret_cast<uint8_t *>(&server_test), sizeof(server_test));
        Serial.println("Save [scope=SERVER_TEST, start=" + String(startByte) + "]");
        EEPROM.put(startByte, server_test);
        EEPROM.put(startByte + sizeof(server_test), scopeChecksum);
    }

    startByte += sizeof(server_test) + sizeof(scopeChecksum);
    if ((scope & SCOPE_TIMER) == SCOPE_TIMER)
    {
        scopeChecksum = checksum(reinterpret_cast<uint8_t *>(&timer), sizeof(timer));
        Serial.println("Save [scope=TIMER, start=" + String(startByte) + "]");
        EEPROM.put(startByte, timer);
        EEPROM.put(startByte + sizeof(timer), scopeChecksum);
    }

    startByte += sizeof(timer) + sizeof(scopeChecksum);
    if ((scope & SCOPE_SETTINGS) == SCOPE_SETTINGS)
    {
        scopeChecksum = checksum(reinterpret_cast<uint8_t *>(&settings), sizeof(settings));
        Serial.println("Save [scope=SETTINGS, start=" + String(startByte) + "]");
        EEPROM.put(startByte, settings);
        EEPROM.put(startByte + sizeof(settings), scopeChecksum);
    }

    Serial.println("Save EEPROM " + String(startByte + sizeof(settings) + sizeof(scopeChecksum)));
    EEPROM.commit();

    GUI.publishStatus();
    return 0;
}

uint8_t ConfigManager::checksum(uint8_t *byteArray, unsigned long length)
{
    uint8_t value = 0;
    unsigned long counter;

    for (counter = 0; counter < length; counter++)
    {
        value += *byteArray;
        byteArray++;
    }

    return (uint8_t)(256 - value);
}

String ConfigManager::converIPv4ToString(uint32_t ip)
{
    String ret = "0.0.0.0";

    if (ip > 0)
    {
        ret = IPAddress(ip).toString();
    }
    return ret;
}

uint32_t ConfigManager::converStringToIPv4(String ipString)
{
    uint32_t ret = 0;
    IPAddress ip;
    if (ip.fromString(ipString))
    {
        ret = ip;
    }

    return ret;
}

String ConfigManager::getJSONString(uint8_t scope)
{
    String JSON;
    StaticJsonDocument<1024> jsonBuffer;

    if ((scope & SCOPE_LEGAL) == SCOPE_LEGAL)
    {
        //legal
        jsonBuffer["userAgreementAccepted"] = (uint8_t)configManager.legal.userAgreementAccepted;
        jsonBuffer["countryCode"] = configManager.legal.countryCode;
    }

    if ((scope & SCOPE_WIFI) == SCOPE_WIFI)
    {
        jsonBuffer["ssid"] = "";
        jsonBuffer["captivePortal"] = (uint8_t)wifiManager.isCaptivePortal();
        jsonBuffer["ssid"] = wifiManager.SSID();
        jsonBuffer["password"] = ""; // v4IpToString(configManager.wifi.password);
        jsonBuffer["useDHCP"] = (uint8_t)configManager.wifi.useDHCP;
        jsonBuffer["fixedIp"] = converIPv4ToString(configManager.wifi.fixedIp);
        jsonBuffer["subnetMask"] = converIPv4ToString(configManager.wifi.subnetMask);
        jsonBuffer["dnsServerIp"] = converIPv4ToString(configManager.wifi.dnsServerIp);
        jsonBuffer["gatewayIp"] = converIPv4ToString(configManager.wifi.gatewayIp);
        jsonBuffer["mdnsName"] = configManager.getDeviceName();
    }

    if ((scope & SCOPE_WIFI_TEST) == SCOPE_WIFI_TEST)
    {
        jsonBuffer["isComplete"] = configManager.wifi_test.isComplete;
    }

    if ((scope & SCOPE_TIME) == SCOPE_TIME)
    {
        jsonBuffer["enableDaylightSavingTime"] = configManager.time.enableDaylightSavingTime;
        jsonBuffer["timeZone"] = configManager.time.timeZone;
        jsonBuffer["useNTP"] = configManager.time.useNTP;
        jsonBuffer["ntpServer"] = configManager.time.ntpServer;
    }

    if ((scope & SCOPE_SERVER) == SCOPE_SERVER)
    {
        jsonBuffer["serverProductId"] = configManager.server.serverProductId;
        jsonBuffer["serverHost"] = String(configManager.server.serverHost);
        jsonBuffer["serverPort"] = configManager.server.serverPort;
        jsonBuffer["measureInterval"] = configManager.server.measureInterval;
    }

    if ((scope & SCOPE_SERVER_TEST) == SCOPE_SERVER_TEST)
    {
        jsonBuffer["isComplete"] = configManager.server_test.isComplete;
    }

    if ((scope & SCOPE_TIMER) == SCOPE_TIMER)
    {
        //TODO: implement later
    }

    if ((scope & SCOPE_SETTINGS) == SCOPE_SETTINGS)
    {
        jsonBuffer["operationMode"] = configManager.settings.operationMode;
        jsonBuffer["powerThresholdHigh"] = configManager.settings.powerThresholdHigh;
        jsonBuffer["powerThresholdLow"] = configManager.settings.powerThresholdLow;
        jsonBuffer["maximalDailyDurationHigh"] = configManager.settings.maximalDailyDurationHigh;
        jsonBuffer["enableStatusLED"] = configManager.settings.enableStatusLED;
    }

    serializeJson(jsonBuffer, JSON);
    Serial.println(JSON);
    return JSON;
}

uint8_t ConfigManager::setPreviousSetupScope()
{
    if (settings.isComplete)
    {
        settings.isComplete = false;
        save(SCOPE_SETTINGS);
        return SCOPE_SERVER_TEST;
    }
    else if (server_test.isComplete)
    {
        server_test.isComplete = false;
        save(SCOPE_SERVER_TEST);
        return SCOPE_SERVER;
    }
    else if (server.isComplete)
    {
        server.isComplete = false;
        save(SCOPE_SERVER);
        return SCOPE_WIFI;
    }
    else if (wifi_test.isComplete)
    {
        wifi_test.isComplete = false;
        save(SCOPE_WIFI_TEST);
        return SCOPE_WIFI;
    }
    else if (wifi.isComplete)
    {
        wifi.isComplete = false;
        save(SCOPE_WIFI);
        return SCOPE_LEGAL;
    }
    else if (legal.isComplete)
    {
        legal.isComplete = false;
        save(SCOPE_LEGAL);
    }
    return 0;
}

int ConfigManager::setJSONString(uint8_t scope, String config)
{
    {
        Serial.println(config);

        DynamicJsonDocument doc(1024);
        IPAddress serverIp;

        deserializeJson(doc, config);
        JsonObject obj = doc.as<JsonObject>();

        IPAddress ip;

        if ((scope & SCOPE_LEGAL) == SCOPE_LEGAL)
        {
            legal.isComplete = false;
            bool tmpUserAgreementAccepted = (bool)obj[String("userAgreementAccepted")].as<uint8_t>();

            if (tmpUserAgreementAccepted)
            {
                legal.isComplete = true;
                legal.userAgreementAccepted = tmpUserAgreementAccepted;
            }
            else
            {
                return 0;
            }
        }

        if ((scope & SCOPE_WIFI) == SCOPE_WIFI)
        {

            wifi.isComplete = false;
            setupSsid = obj[String("ssid")].as<String>();
            setupPassword = obj[String("password")].as<String>();

            wifi.useDHCP = (bool)obj[String("useDHCP")].as<uint8_t>();
            wifi.fixedIp = converStringToIPv4(obj[String("fixedIp")].as<String>());
            wifi.subnetMask = converStringToIPv4(obj[String("subnetMask")].as<String>());
            wifi.gatewayIp = converStringToIPv4(obj[String("gatewayIp")].as<String>());
            wifi.dnsServerIp = converStringToIPv4(obj[String("dnsServerIp")].as<String>());

            if (setupSsid.length() >= 1 && setupSsid.length() <= 32 && setupPassword.length() >= 8 && setupPassword.length() < 100)
            {
                if (wifi.useDHCP)
                {
                    wifi.isComplete = true;
                }
                else if (wifi.fixedIp > 0)
                {
                    wifi.isComplete = true;
                }
                else
                {
                    return 1;
                }
            }
            else
            {
                return 1;
            }
        }

        if ((scope & SCOPE_WIFI_TEST) == SCOPE_WIFI_TEST)
        {
            wifi_test.isComplete = (bool)obj[String("wifiTestIsComplete")].as<uint8_t>();
        }

        if ((scope & SCOPE_TIME) == SCOPE_TIME)
        {
            timer.isComplete = false;
            time.useNTP = (bool)obj[String("useNTP")].as<uint8_t>();
        }

        if ((scope & SCOPE_SERVER) == SCOPE_SERVER)
        {
            server.isComplete = false;
            uint16_t tmpProductId = obj[String("serverProductId")].as<uint16_t>();
            String tmpHost = obj[String("serverHost")].as<String>();
            uint16_t tmpPort = obj[String("serverPort")].as<uint16_t>();
            uint32_t tmpInterval = obj[String("measureInterval")].as<uint32_t>();

            if (tmpProductId > 0 && tmpHost.length() > 0 && tmpPort > 0 && tmpPort <= 65535 && tmpInterval > 10)
            {
                server.serverProductId = tmpProductId;
                strcpy(server.serverHost, tmpHost.c_str());
                server.serverPort = tmpPort;
                server.measureInterval = tmpInterval;
                server.isComplete = true;
                Serial.println("Server setup complete");
            }
            else
            {
                Serial.println("Server setup error: invalid params");
                return 1;
            }
        }

        if ((scope & SCOPE_SERVER_TEST) == SCOPE_SERVER_TEST)
        {
            server_test.isComplete = (bool)obj[String("serverTestIsComplete")].as<uint8_t>();
        }

        if ((scope & SCOPE_TIMER) == SCOPE_TIMER)
        {
            //TODO: implement later
            timer.isComplete = false;
        }

        if ((scope & SCOPE_SETTINGS) == SCOPE_SETTINGS)
        {
            settings.isComplete = false;

            uint8_t tmpProtectionType = obj[String("configurationProtectionType")].as<uint8_t>();
            uint8_t tmpOperationMode = obj[String("operationMode")].as<uint8_t>();
            uint32_t tmpPowerThresholdHigh = obj[String("powerThresholdHigh")].as<uint32_t>();
            uint32_t tmpPowerThresholdLow = obj[String("powerThresholdLow")].as<uint32_t>();
            bool tmpEnableStatusLED = (bool)obj[String("enableStatusLED")].as<uint8_t>();

            if (tmpPowerThresholdLow >= 20 && tmpPowerThresholdLow <= 20000 && tmpPowerThresholdHigh >= 20 && tmpPowerThresholdLow <= 20000 && tmpPowerThresholdLow <= tmpPowerThresholdHigh)
            {
                settings.configurationProtectionType = tmpProtectionType;
                settings.operationMode = tmpOperationMode;
                settings.powerThresholdHigh = tmpPowerThresholdHigh;
                settings.powerThresholdLow = tmpPowerThresholdLow;
                settings.enableStatusLED = tmpEnableStatusLED;
                settings.isComplete = true;
                Serial.println("Settings setup success");
            }
            else
            {
                Serial.println("Settings setup error: invalid params");
                return 1;
            }
        }

        save(scope);
    }

    return 0;
}

ConfigManager configManager;
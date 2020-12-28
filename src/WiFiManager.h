#ifndef __WIFI_MANAGER_H__
#define __WIFI_MANAGER_H__

#include <Arduino.h>
#include <DNSServer.h>
#include <ESP8266mDNS.h>
#include <memory>
class WifiManager
{
private:
    DNSServer *dnsServer;
    String ssid;
    String pass;
    IPAddress ip;
    IPAddress gw;
    IPAddress sub;
    IPAddress dns;
    bool reconnect = false;
    bool inCaptivePortal = false;
    String mDNSName;
    String captivePortalName;
    void startCaptivePortal(String apName);
    void stopCaptivePortal();
    void connectNewWifi(String newSSID, String newPass);
    void storeToEEPROM();
    void startMDNS();
public:
    void begin(String apName);
    void loop();
    void forget();
    bool isCaptivePortal();
    String SSID();
    void setNewWifi(String newSSID, String newPass);
    void setNewWifi(String newSSID, String newPass, String newIp, String newSub, String newGw, String newDns);
};

extern WifiManager WiFiManager;

#endif

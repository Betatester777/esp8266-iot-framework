#include <WiFiManager.h>
#include <ESP8266WiFi.h>
#include <ConfigManager.h>
#include <StatusLEDController.h>
#include <ConnectionStateMachine.h>

//create global object
WifiManager wifiManager;

//function to call in setup
void WifiManager::begin(String deviceName)
{
    captivePortalName = deviceName;

    mDNSName = deviceName;
    mDNSName.toLowerCase();

    WiFi.mode(WIFI_STA);
    WiFi.hostname(deviceName);

    //set static IP if entered
    ip = IPAddress(configManager.wifi.fixedIp);
    gw = IPAddress(configManager.wifi.gatewayIp);
    sub = IPAddress(configManager.wifi.subnetMask);
    dns = IPAddress(configManager.wifi.dnsServerIp);

    if (ip.isSet() || gw.isSet() || sub.isSet() || dns.isSet())
    {
        Serial.println(PSTR("Using static IP"));
        WiFi.config(ip, gw, sub, dns);
    }

    if (WiFi.SSID() != "")
    {
        //trying to fix connection in progress hanging
        ETS_UART_INTR_DISABLE();
        wifi_station_disconnect();
        ETS_UART_INTR_ENABLE();
        WiFi.begin();
    }

    if (WiFi.waitForConnectResult() == WL_CONNECTED)
    {
        //connected
        notifySuccesfullConnection("Connected to stored WiFi details");
    }
    else
    {
        //captive portal
        startCaptivePortal(captivePortalName);
    }
}

void WifiManager::notifySuccesfullConnection(String hint)
{
    connectionStateMachine.trigger(TRIGGER_WIFI_CONNECTION_SUCCESS);
    Serial.println(hint);
    Serial.println(WiFi.localIP());
    startMDNS();
}

void WifiManager::startMDNS()
{
    MDNS.close();

    if (MDNS.begin(mDNSName))
    {
        Serial.println(F("mDNS is running: "));
        Serial.printf("http://%s.local\n", mDNSName.c_str());
    }
    else
    {
        Serial.println("mDNS could not start");
    }
}

//function to forget current WiFi details and start a captive portal
void WifiManager::forget()
{
    WiFi.disconnect();
    startCaptivePortal(captivePortalName);

    //remove IP address from EEPROM
    ip = IPAddress();
    sub = IPAddress();
    gw = IPAddress();
    dns = IPAddress();
    Serial.println(PSTR("Requested to forget WiFi. Started Captive portal."));
}

//function to request a connection to new WiFi credentials
void WifiManager::setNewWifi(String newSSID, String newPass)
{
    setNewWifi(newSSID, newPass, true, 0, 0, 0, 0);
}

//function to request a connection to new WiFi credentials
void WifiManager::setNewWifi(String newSSID, String newPass, bool useDHCP, uint32_t newIp, uint32_t newSub, uint32_t newGw, uint32_t newDns)
{
    ssid = newSSID;
    pass = newPass;

    ip = IPAddress();
    sub = IPAddress();
    gw = IPAddress();
    dns = IPAddress();

    if (newIp > 0)
    {
        ip = IPAddress(newIp);
    }

    if (newSub > 0)
    {
        sub = IPAddress(newSub);
    }

    if (newGw > 0)
    {
        gw = IPAddress(newGw);
    }

    if (newDns > 0)
    {
        dns = IPAddress(newDns);
    }

    reconnect = true;
}

//function to connect to new WiFi credentials
void WifiManager::connectNewWifi(String newSSID, String newPass)
{
    delay(1000);

    Serial.println("WIFI connect [ssid=" + newSSID + ", password=" + newPass + "]");
    //set static IP or zeros if undefined

    WiFi.config(ip, gw, sub, dns);

    //fix for auto connect racing issue
    if (!(WiFi.status() == WL_CONNECTED && (WiFi.SSID() == newSSID)) || ip.v4() != configManager.wifi.fixedIp)
    {
        //trying to fix connection in progress hanging
        ETS_UART_INTR_DISABLE();
        wifi_station_disconnect();
        ETS_UART_INTR_ENABLE();

        //store old data in case new network is wrong
        String oldSSID = WiFi.SSID();
        String oldPSK = WiFi.psk();

        WiFi.begin(newSSID.c_str(), newPass.c_str(), 0, NULL, true);
        delay(2000);

        if (WiFi.waitForConnectResult() != WL_CONNECTED)
        {

            Serial.println(PSTR("New connection unsuccessful"));
            if (!inCaptivePortal)
            {
                WiFi.begin(oldSSID, oldPSK, 0, NULL, true);
                if (WiFi.waitForConnectResult() == WL_CONNECTED)
                {
                    notifySuccesfullConnection("Reconnection successful");
                }
                else
                {
                    Serial.println(PSTR("Reconnection failed too"));
                    startCaptivePortal(captivePortalName);
                }
            }
        }
        else
        {
            if (inCaptivePortal)
            {
                stopCaptivePortal();
            }
            notifySuccesfullConnection("New connection successful");
        }
    }
}

//function to start the captive portal
void WifiManager::startCaptivePortal(String apName)
{
    WiFi.persistent(false);
    // disconnect sta, start ap
    WiFi.disconnect(); //  this alone is not enough to stop the autoconnecter
    WiFi.mode(WIFI_AP);
    WiFi.persistent(true);

    WiFi.softAP(apName);

    dnsServer = new DNSServer();

    /* Setup the DNS server redirecting all the domains to the apIP */
    dnsServer->setErrorReplyCode(DNSReplyCode::NoError);
    dnsServer->start(53, "*", WiFi.softAPIP());

    Serial.println(PSTR("Opened a captive portal"));
    Serial.println(PSTR("192.168.4.1"));
    inCaptivePortal = true;
    connectionStateMachine.trigger(TRIGGER_WIFI_CONNECTION_FAIL);
}

//function to stop the captive portal
void WifiManager::stopCaptivePortal()
{
    WiFi.mode(WIFI_STA);
    delete dnsServer;

    inCaptivePortal = false;
}

//return captive portal state
bool WifiManager::isCaptivePortal()
{
    return inCaptivePortal;
}

//return current SSID
String WifiManager::SSID()
{
    return WiFi.SSID();
}

String WifiManager::scanNetworks()
{
    String json = "[";
    int n = WiFi.scanComplete();
    if (n == -2)
    {
        WiFi.scanNetworks(true);
    }
    else if (n)
    {
        for (int i = 0; i < n; ++i)
        {
            if (i)
                json += ",";
            json += "{";
            json += "\"rssi\":" + String(WiFi.RSSI(i));
            json += ",\"ssid\":\"" + WiFi.SSID(i) + "\"";
            json += ",\"bssid\":\"" + WiFi.BSSIDstr(i) + "\"";
            json += ",\"channel\":" + String(WiFi.channel(i));
            json += ",\"secure\":" + String(WiFi.encryptionType(i));
            json += ",\"hidden\":" + String(WiFi.isHidden(i) ? "true" : "false");
            json += "}";
        }
        WiFi.scanDelete();
        if (WiFi.scanComplete() == -2)
        {
            WiFi.scanNetworks(true);
        }
    }
    json += "]";

    return json;
}

String WifiManager::getMACAddress()
{
    return WiFi.macAddress();
}

//captive portal loop
void WifiManager::loop()
{
    if (inCaptivePortal)
    {
        //captive portal loop
        dnsServer->processNextRequest();
    }

    if (reconnect)
    {
        connectNewWifi(ssid, pass);
        reconnect = false;
    }

    MDNS.update();
}
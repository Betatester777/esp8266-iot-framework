#ifndef __CONFIG_MGR_H__
#define __CONFIG_MGR_H__

#include "IPAddress.h"
#include <generated/config.h>
//data that needs to be persisted for other parts of the framework

#define SIZE_DEVICE_INFO 32 //allocate 32 bytes to have room for future expansion

#define SCOPE_LEGAL 0x01
#define SCOPE_WIFI 0x02
#define SCOPE_WIFI_TEST 0x04
#define SCOPE_TIME 0x08
#define SCOPE_SERVER 0x10
#define SCOPE_SERVER_TEST 0x20
#define SCOPE_TIMER 0x40
#define SCOPE_SETTINGS 0x80

#define SERVER_ADDRESS_TYPE_IP 0
#define SERVER_ADDRESS_TYPE_DNS 1
class ConfigManager
{

public:
    LegalConfig legal;
    WifiConfig wifi;
    WifiTestConfig wifi_test;
    TimeConfig time;
    ServerConfig server;
    ServerTestConfig server_test;
    TimerConfig timer;
    Settings settings;

    String setupSsid;
    String setupPassword;

    bool testConnection;
    String testConnectionResult;

    bool enableMeasurePower;
    uint32_t measuredPower;
    uint8_t outputStatus;

    int begin(int numBytes = 512);
    int load();
    String getDeviceName();
    int save(uint8_t scope);
    void reset(uint8_t scope);
    uint8_t setPreviousSetupScope();
    String getJSONString(uint8_t scope);
    int setJSONString(uint8_t scope, String config);
    String converIPv4ToString(uint32_t ip);
    uint32_t converStringToIPv4(String ipString);

private:
    uint8_t checksum(uint8_t *byteArray, unsigned long length);
};

extern ConfigManager configManager;

#endif
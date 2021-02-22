#ifndef __CONFIG_H__
#define __CONFIG_H__

#define FIRMWARE_VERSION "0.0.1"
#define PRODUCT_NAME "WiFiSolarPlug"
#define CAPTIVE_PORTAL_IP "192.168.4.1"

//Sonoff
//#define RelayPin 12
//#define LEDPinSwitch 13
//#define LEDPinPow 15
//#define ButtonPin 0

//HAMA
#define RelayPin 14
#define LEDPinSwitch 13
#define LEDPinPow 15
#define ButtonPin 3


#define KeyBounce 50
#define ButtonShortPressDuration 200
#define ButtonLongPressDuration 10000
#define ButtonConstantPressDuration 30000
#define ButtonIdleDuration 500
#define ButtonMode INPUT_PULLUP

#define MONDAY 0x01
#define TUESDAY 0x02
#define WEDNESDAY 0x04
#define THURSDAY 0x08
#define FRIDAY 0x10
#define SATURDAY 0x20
#define DAY_OF_WEEK_SUNDAY 0x40

enum ProtectionType
{
	NONE,
	CREDENTIALS,
	HARDWARE
};


struct TimerItem
{
	bool isEnabled;
	bool status; //True=HIGH, false=LOW
	uint16_t beginDayOfYear;
	uint16_t endDayOfYear;
	uint8_t daysOfWeek;
	uint16_t beginTime;
	uint16_t endTime;
};
struct LegalConfig
{
	bool isComplete;
	bool userAgreementAccepted;
	char countryCode[3];
};
struct WifiConfig
{
	bool isComplete;
	bool useDHCP;
	uint32_t fixedIp;
	uint32_t gatewayIp;
	uint32_t subnetMask;
	uint32_t dnsServerIp;
};

struct WifiTestConfig
{
	bool isComplete;
};

struct TimeConfig
{
	bool isComplete;
	char timeZone[6];
	bool enableDaylightSavingTime;
	bool useNTP;
	char ntpServer[100];
};
struct ServerConfig
{
	bool isComplete;
	uint32_t serverProductId;
	char serverHost[255];
	uint16_t serverPort;
	uint32_t measureInterval;
};

struct ServerTestConfig
{
	bool isComplete;
};

struct TimerConfig
{
	bool isComplete;
	TimerItem timers[5];
};
struct Settings
{
	bool isComplete;
	uint8_t configurationProtectionType;
	uint8_t operationMode;
	bool enableStatusLED;
	uint32_t powerThresholdHigh;
	uint32_t powerThresholdLow;
	uint32_t maximalDailyDurationHigh;
};

extern uint32_t configVersion;
extern const LegalConfig legalDefaults;
extern const WifiConfig wifiDefaults;
extern const WifiTestConfig wifiTestDefaults;
extern const TimeConfig timeDefaults;
extern const ServerConfig serverDefaults;
extern const ServerTestConfig serverTestDefaults;
extern const TimerConfig timerDefaults;
extern const Settings settingsDefaults;

#endif
#ifndef __CONFIG_H__
#define __CONFIG_H__

#define FIRMWARE_VERSION "0.0.1"
#define PRODUCT_NAME "WiFiSolarPlug"

#define RelayPin 12
#define LEDPinSwitch 13
#define LEDPinPow 15
#define ButtonPin 0
#define KeyBounce 50              //Millisekunden zwischen 2xtasten
#define KeyLongPressDuration 400 //Millisekunden für langen Tastendruck bei Sonoff Touch als Sender
#define ButtonMode INPUT_PULLUP

struct configData
{
	uint32_t serialNumber;
	uint8_t useNTP;
	uint8_t operationMode;
	uint16_t serverProductId;
	uint8_t serverAddressType;
	char serverIp[15];
	char serverDNS[255];
	uint16_t serverPort; 
	uint32_t powerThresholdHigh;
	uint32_t powerThresholdLow;
	uint32_t measureInterval;
	uint8_t enableStatusLED;
};

extern uint32_t configVersion;
extern const configData defaults;
#endif
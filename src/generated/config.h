#ifndef __CONFIG_H__
#define __CONFIG_H__

struct configData
{
	uint8_t useNTP;
	uint8_t operationMode;
	char serverIp[15];
	uint16_t serverPort; 
	uint32_t powerThresholdHigh;
	uint32_t powerThresholdLow;
	uint32_t measureInterval;
	uint8_t enableStatusLED;
};

extern uint32_t configVersion;
extern const configData defaults;
#endif
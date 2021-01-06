#include <Arduino.h>
#include "config.h"

uint32_t configVersion = 3306642818; //generated identifier to compare config with EEPROM
const LegalConfig legalDefaults PROGMEM = {
		false, //isComplete
		false, //User agreement accepted
		"DE"	 //country code
};

const WifiConfig wifiDefaults PROGMEM = {
		false, //isComplete
		true,	 //use DHCP
		0,		 //Fixed IP
		0,		 //Gateway IP
		0,		 //Subnet mast
		0			 //DNS server IP
};

const TimeConfig timeDefaults PROGMEM = {
		false,				 //isComplete
		"CEST",				 //Time zone abbreviation
		false,				 //Enable Daylight Saving Time
		false,				 //use NTP
		"pool.ntp.org" //NTP server
};

const ServerConfig serverDefaults PROGMEM = {
		false, //isComplete
		9225,	 //Inverter Product ID
		"",		 //Inverter host (ip or dns)
		502,	 //Inverter port
		300		 //Inverter Request interval in sec
};

const ServerTestConfig serverTestDefaults PROGMEM = {
		false //isComplete
};

const TimerConfig timerDefaults PROGMEM = {
		false, //isComplete
		{{false, false, 0, 0, 0, 0, 0},
		 {false, false, 0, 0, 0, 0, 0},
		 {false, false, 0, 0, 0, 0, 0},
		 {false, false, 0, 0, 0, 0, 0},
		 {false, false, 0, 0, 0, 0, 0}} //Empty disabled timers
};

const Settings settingsDefaults PROGMEM = {
		false,
		NONE,	 //Config protection mode
		0,		 //Operation Mode MANUAL
		false, // Enable status LED on Request
		20,		 //Power threshold HIGH
		20,		 //Power threshold LOW
		86400	 //Maximal ON time per day in sec (24 houres)
};
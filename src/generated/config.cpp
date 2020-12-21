#include <Arduino.h>
#include "config.h"

uint32_t configVersion = 3306642818; //generated identifier to compare config with EEPROM

const configData defaults PROGMEM =
{
	0,
	2,
	"0.0.0.0",
	0,
	300,//Seconds 
	1// Enable LED
};
#ifndef STATES_H
#define STATES_H

#include "Fsm.h"
#include <Ticker.h>
#include <configManager.h>
#include "sma_modbus.h"

#define FIRMWARE_VERSION = "0.0.1"

#define RelayPin 12
#define LEDPinSwitch 13
#define LEDPinPow 15
#define ButtonPin 0
#define KeyBounce 50              //Millisekunden zwischen 2xtasten
#define KeyLongPressDuration 1500 //Millisekunden für langen Tastendruck bei Sonoff Touch als Sender

#define MANUAL_OFF "stateOperationMode_ManualOff"
#define MANUAL_ON "stateOperationMode_ManualOn"
#define POWER_OFF "stateOperationMode_PowerOff"
#define POWER_ON "stateOperationMode_PowerOn"

#define TRIGGER_TOGGLE_ON_OFF 0
#define TRIGGER_CHANGE_OPERATION_MODE 1
#define TRIGGER_POWER_HIGH 2
#define TRIGGER_POWER_LOW 3

#define OPERATION_MODE_MANUAL 0
#define OPERATION_MODE_POWER 1

void statusLEDBlink();

int randomInRange(int min, int max);

void onEnter_OperationMode_ManualOff();

void onEnter_OperationMode_ManualOn();

void onEnter_OperationMode_PowerOff();

void onEnter_OperationMode_PowerOn();

void indicateOperationModeChange();

void onTransition_OperationMode_Change();

void readMeasuredPower();

void initFsm();

extern State * stateOperationMode_ManualOff;
extern State * stateOperationMode_ManualOn;
extern State * stateOperationMode_PowerOff;
extern State * stateOperationMode_PowerOn;
extern Fsm * fsmOperationMode;
extern uint32_t measuredPower;
extern bool enableMeasurePower;

#endif
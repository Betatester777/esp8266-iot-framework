#ifndef __STATES_H__
#define __STATES_H__

#include <Ticker.h>
#include <StateMachine.h>
#include <ConfigManager.h>

#define FIRMWARE_VERSION = "0.0.1"

#define RelayPin 12
#define LEDPinSwitch 13
#define LEDPinPow 15
#define ButtonPin 0
#define KeyBounce 50              //Millisekunden zwischen 2xtasten
#define KeyLongPressDuration 400 //Millisekunden für langen Tastendruck bei Sonoff Touch als Sender
#define ButtonMode INPUT_PULLUP

#define MANUAL_OFF "OperationMode_ManualOff"
#define MANUAL_ON "OperationMode_ManualOn"
#define POWER_OFF "OperationMode_PowerOff"
#define POWER_ON "OperationMode_PowerOn"

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

void fsm_setup();

extern State * stateOperationMode_ManualOff;
extern State * stateOperationMode_ManualOn;
extern State * stateOperationMode_PowerOff;
extern State * stateOperationMode_PowerOn;
extern Fsm * fsmOperationMode;
extern uint32_t measuredPower;
extern bool enableMeasurePower;

#endif
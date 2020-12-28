#ifndef __STATES_H__
#define __STATES_H__

#include <Ticker.h>
#include <StateMachine.h>
#include <ConfigManager.h>

#define MANUAL_OFF "OperationMode_ManualOff"
#define MANUAL_ON "OperationMode_ManualOn"
#define POWER_OFF "OperationMode_PowerOff"
#define POWER_ON "OperationMode_PowerOn"

#define TRIGGER_TOGGLE_ON_OFF 0
#define TRIGGER_CHANGE_OPERATION_MODE 1
#define TRIGGER_POWER_HIGH 2
#define TRIGGER_POWER_LOW 3
#define TRIGGER_OFF 4
#define TRIGGER_ON 5
#define TRIGGER_OPERATION_MODE_MANUAL 6
#define TRIGGER_OPERATION_MODE_POWER 7

#define OPERATION_MODE_MANUAL 0
#define OPERATION_MODE_POWER 1

#define OUTPUT_OFF 0
#define OUTPUT_ON 1

void statusLEDBlink();

int randomInRange(int min, int max);

void onEnter_OperationMode_ManualOff();

void onEnter_OperationMode_ManualOn();

void onEnter_OperationMode_PowerOff();

void onEnter_OperationMode_PowerOn();

void indicateOperationModeChange();
void indicateOnOffChange();

void indicatePowerMeasureRequest();

void onTransition_OperationMode_Change();
void onTransition_OnOff_Change();

void readMeasuredPower();

void fsm_setup();

extern State * stateOperationMode_ManualOff;
extern State * stateOperationMode_ManualOn;
extern State * stateOperationMode_PowerOff;
extern State * stateOperationMode_PowerOn;
extern Fsm * fsmOperationMode;
extern uint32_t measuredPower;
extern bool enableMeasurePower;
extern uint8_t outputStatus;

#endif
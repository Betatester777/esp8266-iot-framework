#ifndef __OPERATION_MODE_STATE_MACHINE_H__
#define __OPERATION_MODE_STATE_MACHINE_H__

#include <StateMachineBase.h>

#define MANUAL_OFF "MANUAL_OFF"
#define MANUAL_ON "MANUAL_ON"
#define POWER_OFF "POWER_OFF"
#define POWER_ON "POWER_ON"

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

void onEnter_OperationMode_ManualOff();

void onEnter_OperationMode_ManualOn();

void onEnter_OperationMode_PowerOff();

void onEnter_OperationMode_PowerOn();

void onTransition_OperationMode_Change();

void onTransition_OnOff_Change();

class OperationModeStateMachine :  public StateMachineBase
{
private:
  State *stateManualOff;
  State *stateManualOn;
  State *statePowerOff;
  State *statePowerOn;

public:
  OperationModeStateMachine();
};

extern OperationModeStateMachine operationModeStateMachine;

#endif
#include <States.h>
#include <StatusLEDController.h>

uint32_t measuredPower = 0;
bool enableMeasurePower = false;
uint8_t outputStatus = OUTPUT_OFF;

State *stateOperationMode_ManualOff;
State *stateOperationMode_ManualOn;
State *stateOperationMode_PowerOff;
State *stateOperationMode_PowerOn;
Fsm *fsmOperationMode;


void onEnter_OperationMode_ManualOff()
{
  Serial.println("Entering state: OperationMode_ManualOff");
  if (configManager.data.operationMode != OPERATION_MODE_MANUAL)
  {
    configManager.data.operationMode = OPERATION_MODE_MANUAL;
    configManager.save();
  }
  digitalWrite(RelayPin, LOW);
}

void onEnter_OperationMode_ManualOn()
{
  Serial.println("Entering state: OperationMode_ManualOn");
  if (configManager.data.operationMode != OPERATION_MODE_MANUAL)
  {
    configManager.data.operationMode = OPERATION_MODE_MANUAL;
    configManager.save();
  }
  digitalWrite(RelayPin, HIGH);
}

void onEnter_OperationMode_PowerOff()
{
  Serial.println("Entering state: OperationMode_PowerOff");
  if (configManager.data.operationMode != OPERATION_MODE_POWER)
  {
    configManager.data.operationMode = OPERATION_MODE_POWER;
    configManager.save();
  }
  digitalWrite(RelayPin, LOW);
}

void onEnter_OperationMode_PowerOn()
{
  Serial.println("Entering state: OperationMode_PowerOn");
  if (configManager.data.operationMode != OPERATION_MODE_POWER)
  {
    configManager.data.operationMode = OPERATION_MODE_POWER;
    configManager.save();
  }
  digitalWrite(RelayPin, HIGH);
}

void onTransition_OperationMode_Change()
{
  
}

void onTransition_OnOff_Change()
{

}

void fsm_setup()
{
  pinMode(RelayPin, OUTPUT);

  stateOperationMode_ManualOff = new State(MANUAL_OFF, onEnter_OperationMode_ManualOff, NULL, NULL);
  stateOperationMode_ManualOn = new State(MANUAL_ON, onEnter_OperationMode_ManualOn, NULL, NULL);
  stateOperationMode_PowerOff = new State(POWER_OFF, onEnter_OperationMode_PowerOff, NULL, NULL);
  stateOperationMode_PowerOn = new State(POWER_ON, onEnter_OperationMode_PowerOn, NULL, NULL);

  State *initialState;
  switch (configManager.data.operationMode)
  {
  case OPERATION_MODE_MANUAL:
    initialState = stateOperationMode_ManualOff;
    break;
  case OPERATION_MODE_POWER:
    initialState = stateOperationMode_PowerOff;
    break;
  default:
    initialState = stateOperationMode_ManualOff;
  }

  fsmOperationMode = new Fsm(initialState);

  fsmOperationMode->add_transition(stateOperationMode_ManualOff, stateOperationMode_ManualOn, TRIGGER_TOGGLE_ON_OFF, &onTransition_OnOff_Change);
  fsmOperationMode->add_transition(stateOperationMode_ManualOn, stateOperationMode_ManualOff, TRIGGER_TOGGLE_ON_OFF, &onTransition_OnOff_Change);
  fsmOperationMode->add_transition(stateOperationMode_ManualOn, stateOperationMode_ManualOff, TRIGGER_OFF, &onTransition_OnOff_Change);
  fsmOperationMode->add_transition(stateOperationMode_ManualOff, stateOperationMode_ManualOn, TRIGGER_ON, &onTransition_OnOff_Change);

  fsmOperationMode->add_transition(stateOperationMode_PowerOff, stateOperationMode_PowerOn, TRIGGER_POWER_HIGH, NULL);
  fsmOperationMode->add_transition(stateOperationMode_PowerOn, stateOperationMode_PowerOff, TRIGGER_POWER_LOW, NULL);

  fsmOperationMode->add_transition(stateOperationMode_PowerOff, stateOperationMode_ManualOff, TRIGGER_CHANGE_OPERATION_MODE, &onTransition_OperationMode_Change);
  fsmOperationMode->add_transition(stateOperationMode_ManualOff, stateOperationMode_PowerOff, TRIGGER_CHANGE_OPERATION_MODE, &onTransition_OperationMode_Change);

  fsmOperationMode->add_transition(stateOperationMode_PowerOn, stateOperationMode_ManualOff, TRIGGER_CHANGE_OPERATION_MODE, &onTransition_OperationMode_Change);
  fsmOperationMode->add_transition(stateOperationMode_ManualOn, stateOperationMode_PowerOff, TRIGGER_CHANGE_OPERATION_MODE, &onTransition_OperationMode_Change);

  fsmOperationMode->add_transition(stateOperationMode_PowerOn, stateOperationMode_ManualOff, TRIGGER_OPERATION_MODE_MANUAL, &onTransition_OperationMode_Change);
  fsmOperationMode->add_transition(stateOperationMode_PowerOff, stateOperationMode_ManualOff, TRIGGER_OPERATION_MODE_MANUAL, &onTransition_OperationMode_Change);

  fsmOperationMode->add_transition(stateOperationMode_ManualOff, stateOperationMode_PowerOff, TRIGGER_OPERATION_MODE_POWER, &onTransition_OperationMode_Change);
  fsmOperationMode->add_transition(stateOperationMode_ManualOn, stateOperationMode_PowerOff, TRIGGER_OPERATION_MODE_POWER, &onTransition_OperationMode_Change);

  Serial.println("Setup FSM END");
}
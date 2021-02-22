#include <OperationModeStateMachine.h>
#include <ConfigManager.h>
#include <WebServer.h>
#include <StatusLEDController.h>

OperationModeStateMachine operationModeStateMachine;

void onEnter_ManualOff()
{
  Serial.println("Entering OperationModeState: ManualOff");
  if (configManager.settings.operationMode != OPERATION_MODE_MANUAL)
  {
    configManager.settings.operationMode = OPERATION_MODE_MANUAL;
    configManager.save(SCOPE_SETTINGS);
  }
  digitalWrite(RelayPin, LOW);
  configManager.outputStatus = LOW;
  GUI.publishStatus();
}

void onEnter_ManualOn()
{
  Serial.println("Entering OperationModeState: ManualOn");
  if (configManager.settings.operationMode != OPERATION_MODE_MANUAL)
  {
    configManager.settings.operationMode = OPERATION_MODE_MANUAL;
    configManager.save(SCOPE_SETTINGS);
  }
  digitalWrite(RelayPin, HIGH);
  configManager.outputStatus = HIGH;
  GUI.publishStatus();
}

void onEnter_PowerOff()
{
  Serial.println("Entering OperationModeState: PowerOff");
  if (configManager.settings.operationMode != OPERATION_MODE_POWER)
  {
    configManager.settings.operationMode = OPERATION_MODE_POWER;
    configManager.save(SCOPE_SETTINGS);
  }
  digitalWrite(RelayPin, LOW);
  configManager.outputStatus = LOW;
  GUI.publishStatus();
}

void onEnter_PowerOn()
{
  Serial.println("Entering OperationModeState: PowerOn");
  if (configManager.settings.operationMode != OPERATION_MODE_POWER)
  {
    configManager.settings.operationMode = OPERATION_MODE_POWER;
    configManager.save(SCOPE_SETTINGS);
  }
  digitalWrite(RelayPin, HIGH);
  configManager.outputStatus = HIGH;
  GUI.publishStatus();
}

void onTransition_Change()
{
  statusLEDController.start(ChangeOperationModeConfirmation);
}

void onTransition_OnOff_Change()
{
  //statusLEDController.start(Short);
}

OperationModeStateMachine::OperationModeStateMachine() : StateMachineBase()
{
  pinMode(RelayPin, OUTPUT);

  stateManualOff = new State(MANUAL_OFF, onEnter_ManualOff, NULL, NULL);
  stateManualOn = new State(MANUAL_ON, onEnter_ManualOn, NULL, NULL);
  statePowerOff = new State(POWER_OFF, onEnter_PowerOff, NULL, NULL);
  statePowerOn = new State(POWER_ON, onEnter_PowerOn, NULL, NULL);

  switch (configManager.settings.operationMode)
  {
  case OPERATION_MODE_MANUAL:
    set_initial_state(stateManualOff);
    break;
  case OPERATION_MODE_POWER:
    set_initial_state(statePowerOff);
    break;
  default:
    set_initial_state(stateManualOff);
  }

  add_transition(stateManualOff, stateManualOn, TRIGGER_TOGGLE_ON_OFF, &onTransition_OnOff_Change);
  add_transition(stateManualOn, stateManualOff, TRIGGER_TOGGLE_ON_OFF, &onTransition_OnOff_Change);
  add_transition(stateManualOn, stateManualOff, TRIGGER_OFF, &onTransition_OnOff_Change);
  add_transition(stateManualOff, stateManualOn, TRIGGER_ON, &onTransition_OnOff_Change);

  add_transition(statePowerOff, statePowerOn, TRIGGER_POWER_HIGH, NULL);
  add_transition(statePowerOn, statePowerOff, TRIGGER_POWER_LOW, NULL);

  add_transition(statePowerOff, stateManualOff, TRIGGER_CHANGE_OPERATION_MODE, &onTransition_Change);
  add_transition(stateManualOff, statePowerOff, TRIGGER_CHANGE_OPERATION_MODE, &onTransition_Change);

  add_transition(statePowerOn, stateManualOff, TRIGGER_CHANGE_OPERATION_MODE, &onTransition_Change);
  add_transition(stateManualOn, statePowerOff, TRIGGER_CHANGE_OPERATION_MODE, &onTransition_Change);

  add_transition(statePowerOn, stateManualOff, TRIGGER_OPERATION_MODE_MANUAL, &onTransition_Change);
  add_transition(statePowerOff, stateManualOff, TRIGGER_OPERATION_MODE_MANUAL, &onTransition_Change);

  add_transition(stateManualOff, statePowerOff, TRIGGER_OPERATION_MODE_POWER, &onTransition_Change);
  add_transition(stateManualOn, statePowerOff, TRIGGER_OPERATION_MODE_POWER, &onTransition_Change);

  Serial.println("Setup OperationModeStateMachine done!");
}
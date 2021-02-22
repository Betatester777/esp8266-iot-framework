#include <ConfigurationStateMachine.h>
#include <StatusLEDController.h>

ConfigurationStateMachine configurationStateMachine;

void onEnter_Unconfigured()
{
  Serial.println("Entering ConfigurationState: Unconfigured");
}

void onEnter_Configured()
{
  Serial.println("Entering ConfigurationState: Configured");
}

ConfigurationStateMachine::ConfigurationStateMachine() : StateMachineBase()
{
  stateUnconfigured = new State(UNCONFIGURED, onEnter_Unconfigured, NULL, NULL);
  stateConfigured = new State(CONFIGURED, onEnter_Configured, NULL, NULL);

  set_initial_state(stateUnconfigured);

  add_transition(stateUnconfigured, stateConfigured, TRIGGER_COMPLETE_CONFIGURATION, NULL);
  add_transition(stateConfigured, stateUnconfigured, TRIGGER_DELETE_CONFIGURATION, NULL);
  add_transition(stateConfigured, stateUnconfigured, TRIGGER_UNCOMPLETE_CONFIGURATION, NULL);

  Serial.println("Setup ConfigurationStateMachine done!");
}

#ifndef __CONFIGURATION_STATE_MACHINE_H__
#define __CONFIGURATION_STATE_MACHINE_H__

#include <StateMachineBase.h>

#define UNCONFIGURED "UNCONFIGURED"
#define CONFIGURED "CONFIGURED"

#define TRIGGER_DELETE_CONFIGURATION 0
#define TRIGGER_COMPLETE_CONFIGURATION 1
#define TRIGGER_UNCOMPLETE_CONFIGURATION 2

void onEnter_Unconfigured();
void onEnter_Configured();

class ConfigurationStateMachine : public StateMachineBase
{
private:
  State *stateUnconfigured;
  State *stateConfigured;

public:
  ConfigurationStateMachine();
};

extern ConfigurationStateMachine configurationStateMachine;

#endif
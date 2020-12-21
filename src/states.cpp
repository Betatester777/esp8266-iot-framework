#include "states.h"

int blinkCount = 0;
Ticker tickerReadMesuredPower;
Ticker tickerBlink;
uint32_t measuredPower = 0;
bool enableMeasurePower;

State *stateOperationMode_ManualOff;
State *stateOperationMode_ManualOn;
State *stateOperationMode_PowerOff;
State *stateOperationMode_PowerOn;
Fsm *fsmOperationMode;

SMAModbusSlave *inverterSlave;

void statusLEDBlink()
{
  if (blinkCount > 0)
  {
    digitalWrite(LEDPinSwitch, !digitalRead(LEDPinSwitch));
    blinkCount--;
  }
  else
  {
    if (tickerBlink.active())
    {
      tickerBlink.detach();
    }
  }
}

int randomInRange(int min, int max) //range : [min, max]
{
  static bool first = true;
  if (first)
  {
    srand(time(NULL)); //seeding for the first time only!
    first = false;
  }
  return min + rand() % ((max + 1) - min);
}

void onEnter_OperationMode_ManualOff()
{
  Serial.println("Entering state: OperationMode_ManualOff");
  if (configManager.data.operationMode != OPERATION_MODE_MANUAL)
  {
    configManager.data.operationMode = OPERATION_MODE_MANUAL;
    configManager.save();
  }
  inverterSlave->setRequestInterval(0);
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
  inverterSlave->setRequestInterval(0);
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
  inverterSlave->setRequestInterval(configManager.data.measureInterval);
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
  inverterSlave->setRequestInterval(configManager.data.measureInterval);
  digitalWrite(RelayPin, HIGH);
}

void indicateOperationModeChange()
{
  Serial.println("change operation mode");
  digitalWrite(LEDPinSwitch, HIGH);
  blinkCount = 6;
  tickerBlink.detach();
  tickerBlink.attach_ms(200, statusLEDBlink);
}

void onTransition_OperationMode_Change()
{
  indicateOperationModeChange();
}

void readMeasuredPower(uint32_t watts)
{
  measuredPower = watts;
  Serial.println("Power measuered: " + String(measuredPower));

  if (measuredPower >= configManager.data.powerThreshold)
  {
    fsmOperationMode->trigger(TRIGGER_POWER_HIGH);
    Serial.println("Power measuere: HIGH");
  }
  else
  {
    fsmOperationMode->trigger(TRIGGER_POWER_LOW);
    Serial.println("Power measuere: LOW");
  }
}

void initFsm()
{
  pinMode(LEDPinSwitch, OUTPUT);
  pinMode(RelayPin, OUTPUT);
  pinMode(RelayPin, OUTPUT);
  pinMode(ButtonPin, INPUT_PULLUP);

  digitalWrite(LEDPinSwitch, HIGH);

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

  fsmOperationMode->add_transition(stateOperationMode_ManualOff, stateOperationMode_ManualOn, TRIGGER_TOGGLE_ON_OFF, NULL);
  fsmOperationMode->add_transition(stateOperationMode_ManualOn, stateOperationMode_ManualOff, TRIGGER_TOGGLE_ON_OFF, NULL);

  fsmOperationMode->add_transition(stateOperationMode_PowerOff, stateOperationMode_PowerOn, TRIGGER_POWER_HIGH, NULL);
  fsmOperationMode->add_transition(stateOperationMode_PowerOn, stateOperationMode_PowerOff, TRIGGER_POWER_LOW, NULL);

  fsmOperationMode->add_transition(stateOperationMode_PowerOff, stateOperationMode_ManualOff, TRIGGER_CHANGE_OPERATION_MODE, &onTransition_OperationMode_Change);
  fsmOperationMode->add_transition(stateOperationMode_ManualOff, stateOperationMode_PowerOff, TRIGGER_CHANGE_OPERATION_MODE, &onTransition_OperationMode_Change);

  fsmOperationMode->add_transition(stateOperationMode_PowerOn, stateOperationMode_ManualOff, TRIGGER_CHANGE_OPERATION_MODE, &onTransition_OperationMode_Change);
  fsmOperationMode->add_transition(stateOperationMode_ManualOn, stateOperationMode_PowerOff, TRIGGER_CHANGE_OPERATION_MODE, &onTransition_OperationMode_Change);

  inverterSlave = new SMAModbusSlave(String(configManager.data.serverIp), 0, readMeasuredPower);

  Serial.println("Setup FSM END");
}
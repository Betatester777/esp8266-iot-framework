#include <ConnectionStateMachine.h>
#include <StatusLEDController.h>

ConnectionStateMachine connectionStateMachine;

void onEnter_Unconnected()
{
  Serial.println("Entering ConnectionState: Unconnected");
}

void onEnter_WifiConnected()
{
  Serial.println("Entering ConnectionState: WifiConnected");
}

void onEnter_WifiConnectionFailed()
{
  Serial.println("Entering ConnectionState: WifiConnectionFailed");
}

void onEnter_WifiAndModbusConnected()
{
  Serial.println("Entering ConnectionState: WifiAndModbusConnected");
}

void onEnter_ModbusConnectionFailed()
{
  Serial.println("Entering ConnectionState: ModbusConnectionFailed");
}

ConnectionStateMachine::ConnectionStateMachine() : StateMachineBase()
{
  stateUnconnected = new State(UNCONNECTED, onEnter_Unconnected, NULL, NULL);
  stateWifiConnected = new State(WIFI_CONNECTED, onEnter_WifiConnected, NULL, NULL);
  stateWifiConnectionFailed = new State(WIFI_CONNECTION_FAILED, onEnter_WifiConnectionFailed, NULL, NULL);
  stateWifiAndModbusConnected = new State(WIFI_AND_MODBUS_CONNECTED, onEnter_WifiAndModbusConnected, NULL, NULL);
  stateModbusConnectionFailed = new State(MODBUS_CONNECTION_FAILED, onEnter_ModbusConnectionFailed, NULL, NULL);

  set_initial_state(stateUnconnected);

  add_transition(stateUnconnected, stateWifiConnected, TRIGGER_WIFI_CONNECTION_SUCCESS, NULL);
  add_transition(stateWifiConnected, stateWifiConnectionFailed, TRIGGER_WIFI_CONNECTION_FAIL, NULL);
  add_transition(stateWifiConnectionFailed, stateWifiConnected, TRIGGER_WIFI_CONNECTION_SUCCESS, NULL);

  add_transition(stateWifiConnected, stateWifiAndModbusConnected, TRIGGER_MODBUS_CONNECTION_SUCCESS, NULL);
  add_transition(stateWifiAndModbusConnected, stateModbusConnectionFailed, TRIGGER_MODBUS_CONNECTION_FAIL, NULL);
  add_transition(stateModbusConnectionFailed, stateWifiAndModbusConnected, TRIGGER_MODBUS_CONNECTION_SUCCESS, NULL);
  add_transition(stateWifiAndModbusConnected, stateWifiConnectionFailed, TRIGGER_WIFI_CONNECTION_FAIL, NULL);

  Serial.println("Setup ConnectionStateMachine done!");
}
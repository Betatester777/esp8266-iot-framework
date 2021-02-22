#ifndef __CONNECTION_STATE_MACHINE_H__
#define __CONNECTION_STATE_MACHINE_H__

#include <StateMachineBase.h>

#define UNCONNECTED "UNCONNECTED"
#define WIFI_CONNECTED "WIFI_CONNECTED"
#define WIFI_CONNECTION_FAILED "WIFI_CONNECTION_FAILED"
#define WIFI_AND_MODBUS_CONNECTED "WIFI_AND_MODBUS_CONNECTED"
#define MODBUS_CONNECTION_FAILED "MODBUS_CONNECTION_FAILED"

#define TRIGGER_WIFI_CONNECTION_SUCCESS 2
#define TRIGGER_WIFI_CONNECTION_FAIL 3
#define TRIGGER_MODBUS_CONNECTION_SUCCESS 4
#define TRIGGER_MODBUS_CONNECTION_FAIL 5

void onEnter_Unconnected();
void onEnter_WifiConnected();
void onEnter_WifiConnectionFailed();
void onEnter_WifiAndModbusConnected();
void onEnter_ModbusConnectionFailed();

class ConnectionStateMachine : public StateMachineBase
{
private:
  State *stateUnconnected;
  State *stateWifiConnected;
  State *stateWifiConnectionFailed;
  State *stateWifiAndModbusConnected;
  State *stateModbusConnectionFailed;

public:
  ConnectionStateMachine();
};

extern ConnectionStateMachine connectionStateMachine;

#endif
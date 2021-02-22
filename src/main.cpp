#include <Arduino.h>
#include "LittleFS.h"
#include <WiFiManager.h>
#include <WebServer.h>
#include <Updater.h>
#include <Fetch.h>
#include <ConfigManager.h>
#include <TimeSync.h>
#include <ConfigurationStateMachine.h>
#include <ConnectionStateMachine.h>
#include <OperationModeStateMachine.h>
#include <PushButtonHandler.h>
#include <StatusLEDController.h>
#include <SMA/SMAModbusSlave.h>

String lastOperationState = "";

void onPowerChanged(uint32_t oldValue, uint32_t newValue)
{
  configManager.measuredPower = newValue;
  Serial.println("Power measured: " + String(configManager.measuredPower));

  GUI.publishStatus();

  if (configManager.measuredPower > configManager.settings.powerThresholdHigh)
  {
    operationModeStateMachine.trigger(TRIGGER_POWER_HIGH);
  }
  if (configManager.measuredPower < configManager.settings.powerThresholdLow)
  {
    operationModeStateMachine.trigger(TRIGGER_POWER_LOW);
  }
}

void onButtonEvent(int event)
{
  switch (event)
  {
  case Release:
    Serial.println("ButtonEvent [Release]");
    break;
  case ShortPressDetect:
    Serial.println("ButtonEvent [ShortPressDetect]");

    break;
  case ShortPressConfirm:
    Serial.println("ButtonEvent [ShortPressConfirm]");
    operationModeStateMachine.trigger(TRIGGER_TOGGLE_ON_OFF);
    break;
  case ShortPressConfirm_x2:
    Serial.println("ButtonEvent [ShortPressConfirm_x2]");
    statusLEDController.start(Short_x2);
    break;
  case ShortPressConfirm_x3:
    Serial.println("ButtonEvent [ShortPressConfirm_x3]");
    statusLEDController.start(Short_x3);
    break;
  case ShortPressConfirm_x4:
    Serial.println("ButtonEvent [ShortPressConfirm_x4]");
    statusLEDController.start(FastBlinking);
    break;
  case ShortPressConfirm_x5:
    Serial.println("ButtonEvent [ShortPressConfirm_x5]");
    statusLEDController.start(SlowBlinking);
    break;
  case LongPressDetect:
    Serial.println("ButtonEvent [LongPressDetect]");
    statusLEDController.start(Long);
    break;
  case LongPressConfirm:
    Serial.println("ButtonEvent [LongPressConfirm]");
    operationModeStateMachine.trigger(TRIGGER_CHANGE_OPERATION_MODE);
    break;
  case ConstantPressDetect:
    Serial.println("ButtonEvent [ConstantPressDetect]");
    statusLEDController.start(Constant);
    break;
  case ConstantPressConfirm:
    Serial.println("ButtonEvent [ConstantPressConfirm]");
    Serial.println("Factory reset...");
    configManager.reset(SCOPE_LEGAL | SCOPE_WIFI | SCOPE_WIFI_TEST | SCOPE_SERVER | SCOPE_SERVER_TEST | SCOPE_TIME | SCOPE_TIMER | SCOPE_SETTINGS);
    break;
  }
}

void setup()
{
  Serial.begin(115200);
  LittleFS.begin();
  configurationStateMachine.begin();
  GUI.begin();
  configManager.begin();
  String deviceName = configManager.getDeviceName();
  connectionStateMachine.begin();
  wifiManager.begin(deviceName);
  timeSync.begin();
  pushButtonHandler.begin(ButtonPin, ButtonMode, &onButtonEvent);
  statusLEDController.begin(LEDPinSwitch, true);
  operationModeStateMachine.begin();
  smaModbusSlave = new SMAModbusSlave(String(configManager.server.serverHost),
                                      configManager.server.serverPort,
                                      30775,
                                      2,
                                      &onPowerChanged);
}

void loop()
{
  statusLEDController.loop();
  configurationStateMachine.loop();
  connectionStateMachine.loop();
  operationModeStateMachine.loop();
  wifiManager.loop();
  updater.loop();
  pushButtonHandler.loop();
  String configurationState = configurationStateMachine.get_current_state_key();
  String connectionState = connectionStateMachine.get_current_state_key();
  String operationState = operationModeStateMachine.get_current_state_key();

  if (lastOperationState != operationState && (operationState == MANUAL_OFF || operationState == MANUAL_ON))
  {
    smaModbusSlave->stopTimer();
  }

  if (configManager.testConnection)
  {
    Serial.println("Run connection test");
    smaModbusSlave->setHostAndPort(configManager.server.serverHost, configManager.server.serverPort);
    configManager.testConnectionResult = smaModbusSlave->runTest();
    Serial.println("Test result=" + configManager.testConnectionResult);
    configManager.testConnection = false;
    auto callback = serverConnectionTestQueue.back();
    callback();
  }
  else if (strcmp(configurationState.c_str(), CONFIGURED) == 0 &&
           (strcmp(connectionState.c_str(), WIFI_CONNECTED) == 0 ||
            strcmp(connectionState.c_str(), WIFI_AND_MODBUS_CONNECTED) == 0 ||
            strcmp(connectionState.c_str(), MODBUS_CONNECTION_FAILED) == 0) &&
           (operationState == POWER_OFF || operationState == POWER_ON))
  {
    if (smaModbusSlave->isTimerExpired() || lastOperationState == MANUAL_OFF || lastOperationState == MANUAL_ON)
    {
      Serial.println("Timer is expired red MODBUS value");
      smaModbusSlave->setHostAndPort(configManager.server.serverHost, configManager.server.serverPort);
      smaModbusSlave->readRegister();

      if (configManager.settings.enableStatusLED)
      {
        statusLEDController.start(Short);
      }

      smaModbusSlave->startTimer(configManager.server.measureInterval * 1000);
    }
  }

  lastOperationState = operationState;

  configManager.outputStatus = digitalRead(RelayPin);

  delay(1);
}
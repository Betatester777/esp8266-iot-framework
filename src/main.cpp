#include <Arduino.h>
#include "LittleFS.h"
#include <WiFiManager.h>
#include <WebServer.h>
#include <Updater.h>
#include <Fetch.h>
#include <ConfigManager.h>
#include <TimeSync.h>
#include <States.h>
#include <PushButtonHandler.h>
#include <StatusLEDController.h>
#include <SMA/SMAModbusSlave.h>

void onPowerChanged(uint32_t oldValue, uint32_t newValue)
{
  measuredPower = newValue;
  Serial.println("Power measured: " + String(measuredPower));

  if (measuredPower > configManager.settings.powerThresholdHigh)
  {
    fsmOperationMode->trigger(TRIGGER_POWER_HIGH);
  }
  if (measuredPower < configManager.settings.powerThresholdLow)
  {
    fsmOperationMode->trigger(TRIGGER_POWER_LOW);
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
    fsmOperationMode->trigger(TRIGGER_TOGGLE_ON_OFF);
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
    fsmOperationMode->trigger(TRIGGER_CHANGE_OPERATION_MODE);
    smaModbusSlave->resetTimer(configManager.server.measureInterval * 1000);
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

String serverHost;

void setup()
{
  Serial.begin(115200);
  LittleFS.begin();
  GUI.begin();
  configManager.begin();

  //Create device ID from serial number

  String deviceName = configManager.getDeviceName();
  wifiManager.begin(deviceName);
  timeSync.begin();
  fsm_setup();
  pushButtonHandler.begin(ButtonPin, ButtonMode, &onButtonEvent);
  statusLEDController.begin(LEDPinSwitch, true);

  smaModbusSlave = new SMAModbusSlave(String(configManager.server.serverHost),
                                      configManager.server.serverPort,
                                      30775,
                                      2,
                                      &onPowerChanged);
  smaModbusSlave->resetTimer(configManager.server.measureInterval * 1000);
}

void loop()
{
  //software interrupts
  wifiManager.loop();
  updater.loop();
  pushButtonHandler.loop();
  statusLEDController.loop();
  fsmOperationMode->loop();

  if (configManager.testConnection)
  {
    Serial.println("Run connection test");
    smaModbusSlave->setHostAndPort(configManager.server.serverHost, configManager.server.serverPort);
    configManager.testConnectionResult = smaModbusSlave->_runTest();
    Serial.println("Test result=" + configManager.testConnectionResult);
    configManager.testConnection = false;
    auto callback = serverConnectionTestQueue.back();
    callback();
  }
  else if (smaModbusSlave->isTimerExpired())
  {
    smaModbusSlave->startTimer(configManager.server.measureInterval * 1000);
    String stateKey = fsmOperationMode->get_current_state_key();
    if (stateKey == POWER_OFF || stateKey == POWER_ON)
    {
      smaModbusSlave->setHostAndPort(configManager.server.serverHost, configManager.server.serverPort);

      int result=smaModbusSlave->_readRegister();

      if (configManager.settings.enableStatusLED)
      {
        statusLEDController.start(Short);
      }

    }
  }

  outputStatus = digitalRead(RelayPin);

  delay(1);
}
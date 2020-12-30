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

unsigned long modbusRequestDelay;
unsigned long modbusRequestTimer;

void onPowerChanged(uint32_t oldValue, uint32_t newValue)
{
  measuredPower = newValue;
  Serial.println("Power measured: " + String(measuredPower));

  if (measuredPower > configManager.data.powerThresholdHigh)
  {
    fsmOperationMode->trigger(TRIGGER_POWER_HIGH);
  }
  if (measuredPower < configManager.data.powerThresholdLow)
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
      statusLEDController.start(Short);
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
    break;
  case ConstantPressDetect:
    Serial.println("ButtonEvent [ConstantPressDetect]");
    statusLEDController.start(Constant);
    break;
  case ConstantPressConfirm:
    Serial.println("ButtonEvent [ConstantPressConfirm]");
    break;
  }
}

SMAModbusSlave *smaModbusSlave;
String serverHost;

void setup()
{
  Serial.begin(115200);
  modbusRequestTimer = 0;
  LittleFS.begin();
  GUI.begin();
  configManager.begin();

  //Create device ID from serial number
  String serialNumberString = String(configManager.data.serialNumber);
  String deviceName = String(configManager.data.serialNumber);
  deviceName = String(PRODUCT_NAME) + "-" + deviceName.substring(deviceName.length() - 3);
  WiFiManager.begin(deviceName);
  timeSync.begin();
  fsm_setup();
  pushButtonHandler.begin(ButtonPin, ButtonMode, &onButtonEvent);
  statusLEDController.begin(LEDPinSwitch, true);

  serverHost = String(configManager.data.serverIp);
  if (configManager.data.serverAddressType == 1)
  {
    serverHost = String(configManager.data.serverDNS);
  }
  smaModbusSlave = new SMAModbusSlave(serverHost,
                                      configManager.data.serverPort,
                                      0,
                                      30775,
                                      2,
                                      &onPowerChanged);
}

void loop()
{
  //software interrupts
  WiFiManager.loop();
  updater.loop();
  pushButtonHandler.loop();
  statusLEDController.loop();
  fsmOperationMode->loop();
  modbusRequestDelay = configManager.data.measureInterval * 1000;
  if (millis() >= (modbusRequestTimer + modbusRequestDelay) && modbusRequestDelay > 0)
  {
    modbusRequestTimer += modbusRequestDelay;
    String stateKey = fsmOperationMode->get_current_state_key();
    if (stateKey == POWER_OFF || stateKey == POWER_ON)
    {

      if (configManager.data.serverAddressType == SERVER_ADDRESS_TYPE_DNS)
      {
        smaModbusSlave->setHostAndPort(configManager.data.serverDNS, configManager.data.serverPort);
      }
      else
      {
        smaModbusSlave->setHostAndPort(configManager.data.serverIp, configManager.data.serverPort);
      }

      smaModbusSlave->_readRegister();

      if (configManager.data.enableStatusLED)
      {
        statusLEDController.start(Short);
      }
    }
  }

  outputStatus = digitalRead(RelayPin);

  delay(1);
}
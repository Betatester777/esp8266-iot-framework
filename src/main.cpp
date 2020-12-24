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
#include <SMA/SMAModbusSlave.h>


int period = 5000;
unsigned long time_now = 0;

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

void onButtonEvent(bool shortPress, bool longPress)
{
  if(shortPress){
    fsmOperationMode->trigger(TRIGGER_TOGGLE_ON_OFF);
    return;
  }
  if(longPress){
    fsmOperationMode->trigger(TRIGGER_CHANGE_OPERATION_MODE);    
    return;
  }
}

SMAModbusSlave *smaModbusSlave;

void setup()
{
  Serial.begin(115200);
  LittleFS.begin();
  GUI.begin();
  configManager.begin();
  WiFiManager.begin("WiFiSolarPlug");
  timeSync.begin();
  fsm_setup();
  pushButtonHandler.begin(ButtonPin, ButtonMode, &onButtonEvent);
  smaModbusSlave = new SMAModbusSlave(String(configManager.data.serverIp),
                                      configManager.data.serverPort,
                                      0,
                                      30775,
                                      4,
                                      &onPowerChanged);
}

void loop()
{
  //software interrupts
  WiFiManager.loop();
  updater.loop();
  pushButtonHandler.loop();
  fsmOperationMode->loop();

  period = configManager.data.measureInterval * 1000;
  if (millis() >= (time_now + period))
  {
    time_now += period;
    String stateKey=fsmOperationMode->get_current_state_key();
    if(stateKey==POWER_OFF || stateKey==POWER_ON){
      smaModbusSlave->_readRegister();
    }
  }

  delay(1);
}
#include <Arduino.h>
#include "LittleFS.h"
#include <WiFiManager.h>
#include <WebServer.h>
#include <Updater.h>
#include <Fetch.h>
#include <ConfigManager.h>
#include <TimeSync.h>
#include <States.h>
#include <SMA/SMAModbusSlave.h>

unsigned long buttonTimer = 0;
unsigned long longPressTime = 500;

int period = 5000;
unsigned long time_now = 0;

boolean buttonActive = false;
boolean longPressActive = false;

void onPowerChanged(uint32_t oldValue, uint32_t newValue)
{
  measuredPower = newValue;
  Serial.println("Power measured: " + String(measuredPower));

  if (measuredPower > configManager.data.powerThresholdHigh)
  {
    fsmOperationMode->trigger(TRIGGER_POWER_HIGH);
    Serial.println("Power measure: HIGH");
  }

  if (measuredPower < configManager.data.powerThresholdLow)
  {
    fsmOperationMode->trigger(TRIGGER_POWER_LOW);
    Serial.println("Power measure: LOW");
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

  if (digitalRead(ButtonPin) == LOW) //Invert for pullup_button
  {
    if (buttonActive == false)
    {
      buttonActive = true;
      buttonTimer = millis();
    }
    if ((millis() - buttonTimer) > longPressTime && longPressActive == false)
    {
      longPressActive = true;
    }
  }
  else
  {
    if (buttonActive == true)
    {
      if (longPressActive == true)
      {
        longPressActive = false;
        Serial.println(F("Button event: LONG_PRESS"));
        fsmOperationMode->trigger(TRIGGER_CHANGE_OPERATION_MODE);
      }
      else
      {
        Serial.println(F("Button event: SHORT_PRESS"));
        fsmOperationMode->trigger(TRIGGER_TOGGLE_ON_OFF);
      }
      buttonActive = false;
    }
  }

  delay(1);
}
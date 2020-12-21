#include <Arduino.h>
#include "LittleFS.h"

#include "WiFiManager.h"
#include "webServer.h"
#include "updater.h"
#include "fetch.h"
#include "configManager.h"
#include "timeSync.h"
#include "states.h"
#include "multipress.h"

void handleButtonActions(const int countPresses)  // example of registering Multi-Presses
{
  Serial.print(F("Button B:\t"));
  switch(countPresses)
  {
    case -1:
      Serial.println(F("Button event: LONG_PRESS"));
      fsmOperationMode->trigger(TRIGGER_CHANGE_OPERATION_MODE);
      break;
    case 1:
      Serial.println(F("Button event: SHORT_PRESS"));
      fsmOperationMode->trigger(TRIGGER_TOGGLE_ON_OFF);
      break;
  }
}

SimplePress pushButtons[]= {{ButtonPin, 500, handleButtonActions}};

void setup()
{
    Serial.begin(115200);
    LittleFS.begin();
    GUI.begin();
    configManager.begin();
    WiFiManager.begin("WiFiSolarPlug");
    timeSync.begin();

    initFsm();

    SimplePress::beginAll();
    SimplePress::setDebounceAll(KeyBounce);
}

void loop()
{
    //software interrupts
    WiFiManager.loop();
    updater.loop();
    fsmOperationMode->run_machine();
    
    SimplePress::update();
}

#include <PushButtonHandler.h>
#include <States.h>

PushButtonHandler pushButtonHandler;

void PushButtonHandler::begin(uint8_t buttonPin, uint8_t buttonMode, void (*onButtonEvent)(bool shortPress, bool longPress))
{
  _shortPressActive = false;
  _buttonTimer = 0;
  _buttonPin = buttonPin;
  _buttonMode = buttonMode;
  _onButtonEvent = onButtonEvent;
  _longPressTime = KeyLongPressDuration;
  pinMode(buttonPin, buttonMode);
}

void PushButtonHandler::loop()
{
  bool buttonPressed;

  if (_buttonMode == INPUT)
  {
    buttonPressed = digitalRead(_buttonPin) == HIGH;
  }
  else
  {
    //Invert for pullup_button
    buttonPressed = digitalRead(_buttonPin) == LOW;
  }

  if (buttonPressed)
  {
    if (_shortPressActive == false)
    {
      _shortPressActive = true;
      _buttonTimer = millis();
    }
    
    if (((millis() - _buttonTimer) > _longPressTime) && _longPressActive == false)
    {
      _longPressActive = true;
      Serial.println(F("Button event: LONG_PRESS"));
      _onButtonEvent(false, true);
    }
  }
  else
  {
    if (_shortPressActive == true)
    {
      if (_longPressActive == true)
      {
        _longPressActive = false;
      }
      else
      {
        Serial.println(F("Button event: SHORT_PRESS"));
        _onButtonEvent(true, false);
      }
      _shortPressActive = false;
    }
  }
}
#include <PushButtonHandler.h>
#include <OperationModeMachine.h>

void print_uint64_t(uint64_t num)
{

  char rev[128];
  char *p = rev + 1;

  while (num > 0)
  {
    *p++ = '0' + (num % 10);
    num /= 10;
  }
  p--;
  /*Print the number which is now in reverse*/
  while (p > rev)
  {
    Serial.print(*p--);
  }
}

PushButtonHandler pushButtonHandler;

void PushButtonHandler::begin(uint8_t buttonPin, uint8_t buttonMode, void (*onButtonEvent)(int eventType))
{
  _bootTimeStamp = millis();
  _buttonPin = buttonPin;
  _buttonMode = buttonMode;
  pinMode(buttonPin, buttonMode);
  _onButtonEvent = onButtonEvent;
}

void PushButtonHandler::loop()
{
  bool isButtonPressed;
  uint32_t buttonPressDuration = millis() - _buttonPressTimeStamp;
  uint32_t buttonReleaseDuration = millis() - _buttonReleaseTimeStamp;
  bool isPressedReleasedChanged = false;

  if (_buttonMode == INPUT)
  {
    isButtonPressed = digitalRead(_buttonPin) == HIGH;
  }
  else
  {
    //Invert for pullup_button
    isButtonPressed = digitalRead(_buttonPin) == LOW;
  }

  isPressedReleasedChanged = _lastIsButtonPressed != isButtonPressed;

  if (isPressedReleasedChanged)
  {
    if (isButtonPressed)
    {
      //On button down
      _buttonPressTimeStamp = millis();
    }
    else
    {
      //On button up
      _buttonReleaseTimeStamp = millis();

      if (_constantPressDetected)
      {
        _onButtonEvent(ConstantPressConfirm);
      }
      else if (_longPressDetected)
      {
        _onButtonEvent(LongPressConfirm);
      }
      else if (_shortPressDetected)
      {
        _shortPressCount++;
        Serial.println("ShortPressCount: " + String(_shortPressCount));
      }

      _shortPressDetected = false;
      _longPressDetected = false;
      _constantPressDetected = false;
    }
  }
  else
  {
    if (isButtonPressed)
    {
      //while button hold down
      if (buttonPressDuration <= ButtonShortPressDuration && !_shortPressDetected)
      {
        _onButtonEvent(ShortPressDetect);
        _shortPressDetected = true;
      }
      else if (buttonPressDuration >= ButtonLongPressDuration && !_longPressDetected)
      {
        _onButtonEvent(LongPressDetect);
        _longPressDetected = true;
      }
      else if (buttonPressDuration >= ButtonConstantPressDuration && !_constantPressDetected)
      {
        _onButtonEvent(ConstantPressDetect);
        _constantPressDetected = true;
      }
    }
    else
    {
      //while button idle
      if (buttonReleaseDuration > ButtonIdleDuration && _shortPressCount > 0)
      {
        Serial.println("idle is long enough trigger confirmed shortpresses");
        switch (_shortPressCount)
        {
        case 1:
          _onButtonEvent(ShortPressConfirm);
          break;
        case 2:
          _onButtonEvent(ShortPressConfirm_x2);
          break;
        case 3:
          _onButtonEvent(ShortPressConfirm_x3);
          break;
        case 4:
          _onButtonEvent(ShortPressConfirm_x4);
          break;
        case 5:
          _onButtonEvent(ShortPressConfirm_x5);
          break;
        default:
          Serial.println("invalid short presses count: " + _shortPressCount);
        }
        _shortPressCount = 0;
      }
    }
  }
  _lastIsButtonPressed = isButtonPressed;
}
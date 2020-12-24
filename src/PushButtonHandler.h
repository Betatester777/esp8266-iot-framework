#ifndef __PUSH_BUTTON_HANDLER_H__
#define __PUSH_BUTTON_HANDLER_H__

#include <Arduino.h>

class PushButtonHandler
{
private:
    uint8_t _buttonPin;
    uint8_t _buttonMode;
    unsigned long _buttonTimer;
    unsigned long _longPressTime;
    bool _shortPressActive;
    bool _longPressActive;
    void (*_onButtonEvent)(bool shortPress, bool longPress);
public:
    void begin(uint8_t buttonPin, uint8_t buttonMode, void (*onButtonEvent)(bool shortPress, bool longPress));
    void loop();
};

extern PushButtonHandler pushButtonHandler;

#endif
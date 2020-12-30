#ifndef __PUSH_BUTTON_HANDLER_H__
#define __PUSH_BUTTON_HANDLER_H__

#include <Arduino.h>

class PushButtonHandler
{
private:
    uint64_t _bootTimeStamp;
    uint64_t _buttonPressTimeStamp;
    uint64_t _buttonReleaseTimeStamp;
    uint8_t _buttonPin;
    uint8_t _buttonMode;
    uint8_t _shortPressCount;
    bool _shortPressDetected;
    bool _longPressDetected;
    bool _constantPressDetected;

    bool _lastIsButtonPressed;

    void (*_onButtonEvent)(int eventType);
public:
    void begin(uint8_t buttonPin, uint8_t buttonMode, void (*onButtonEvent)(int eventType));
    void loop();
};

extern PushButtonHandler pushButtonHandler;

#endif
#ifndef __STATUS_LED_CONTROLLER_H__
#define __STATUS_LED_CONTROLLER_H__

#include <Arduino.h>

enum LEDSequenceTypes
{
    Idle,
    Short,
    Short_x2,
    Short_x3,
    Long,
    Constant,
    FastBlinking,
    SlowBlinking
};

class SequenceFrame
{
public:
    SequenceFrame(uint8_t status, unsigned int duration);
    uint8_t _status;
    unsigned int _duration;
};

class Sequence
{
public:
    public:
    Sequence(uint8_t type);
    Sequence(uint8_t type, bool playInLoop);
    void addFrame(uint8_t status, unsigned int duration);
    void first();
    bool next();
    SequenceFrame getCurrent();
    uint8_t getType();
    private:
    uint8_t _type;
    std::vector<SequenceFrame> _frames;
    int _currentFrameIndex;
    bool _playInLoop;
};

class StatusLEDController
{
private:
    uint8_t _pin;
    bool _invert;
    bool _isRunning;
    std::vector<Sequence> _sequences;
    int _sequenceIndex;
    unsigned long _startTimeStamp;

public:
    void begin(uint8_t ledPin, bool invert);
    void start(uint8_t sequenceType);
    void stop();
    void loop();
};

extern StatusLEDController statusLEDController;

#endif
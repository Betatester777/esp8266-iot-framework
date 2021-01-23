#include <StatusLEDController.h>
#include <States.h>

StatusLEDController statusLEDController;

SequenceFrame::SequenceFrame(uint8_t status, unsigned int duration)
{
  _status = status;
  _duration = duration;
}

Sequence::Sequence(uint8_t type)
{
  _type = type;
  _playInLoop = false;
}

Sequence::Sequence(uint8_t type, bool playInLoop)
{
  _type = type;
  _playInLoop = playInLoop;
}

uint8_t Sequence::getType()
{
  return _type;
}

void Sequence::addFrame(uint8_t status, unsigned int duration)
{
  _frames.push_back(SequenceFrame(status, duration));
}

void Sequence::first()
{
  _currentFrameIndex = 0;
  //Serial.println("LEDSequence frame " + String(_currentFrameIndex) + " of " + String(_frames.size()));
}

bool Sequence::next()
{

  if ((_currentFrameIndex + 1) < (int)_frames.size())
  {
    _currentFrameIndex++;
    //Serial.println("LEDSequence next frame " + String(_currentFrameIndex) + " of " + String(_frames.size()));
    return true;
  }
  else if (_playInLoop)
  {
    _currentFrameIndex = 0;
    //Serial.println("LEDSequence next frame " + String(_currentFrameIndex) + " of " + String(_frames.size()));
    return true;
  }

  return false;
}

SequenceFrame Sequence::getCurrent()
{
  return _frames[_currentFrameIndex];
}

void StatusLEDController::begin(uint8_t ledPin, bool invert)
{
  _pin = ledPin;
  _invert = invert;
  pinMode(_pin, OUTPUT);

  if (invert)
  {
    digitalWrite(_pin, HIGH);
  }
  else
  {
    digitalWrite(_pin, LOW);
  }

  _sequences.push_back(Sequence(Idle));
  _sequences[0].addFrame(LOW, 1);

  _sequences.push_back(Sequence(Short));
  _sequences[1].addFrame(HIGH, 200);
  _sequences[1].addFrame(LOW, 200);

  _sequences.push_back(Sequence(Short_x2));
  for (int i = 0; i < 2; i++)
  {
    _sequences[2].addFrame(HIGH, 200);
    _sequences[2].addFrame(LOW, 200);
  }

  _sequences.push_back(Sequence(Short_x3));
  for (int i = 0; i < 3; i++)
  {
    _sequences[3].addFrame(HIGH, 200);
    _sequences[3].addFrame(LOW, 200);
  }

  _sequences.push_back(Sequence(Long));
  _sequences[4].addFrame(HIGH, 1000);
  _sequences[4].addFrame(LOW, 100);

  _sequences.push_back(Sequence(Constant));
  for (int i = 0; i < 20; i++)
  {
    _sequences[5].addFrame(HIGH, 100);
    _sequences[5].addFrame(LOW, 50);
  }

  _sequences.push_back(Sequence(FastBlinking, true));
  _sequences[6].addFrame(HIGH, 200);
  _sequences[6].addFrame(LOW, 200);

  _sequences.push_back(Sequence(SlowBlinking, true));
  _sequences[7].addFrame(HIGH, 800);
  _sequences[7].addFrame(LOW, 200);

  //Device modes indications and confirmations

  _sequences.push_back(Sequence(FactorySettingsIndication, true));
  _sequences[2].addFrame(HIGH, 1000);
  _sequences[2].addFrame(LOW, 1000);

  _sequences.push_back(Sequence(WifiConnectionSuccessIndication));
  for (int i = 0; i < 3; i++)
  {
    _sequences[3].addFrame(HIGH, 1000);
    _sequences[3].addFrame(LOW, 1000);
  }

  _sequences.push_back(Sequence(WifiConnectionFailedIndication));
  for (int i = 0; i < 2; i++)
  {
    _sequences[3].addFrame(HIGH, 500);
    _sequences[3].addFrame(LOW, 500);
  }
  _sequences[3].addFrame(LOW, LED_INDICATION_PAUSE);

  _sequences.push_back(Sequence(ModbusConnectionFailedIndication));
  for (int i = 0; i < 4; i++)
  {
    _sequences[3].addFrame(HIGH, 500);
    _sequences[3].addFrame(LOW, 500);
  }
  _sequences[3].addFrame(LOW, LED_INDICATION_PAUSE);

  _sequences.push_back(Sequence(FactoryResetConfirmation));
  for (int i = 0; i < 20; i++)
  {
    _sequences[3].addFrame(HIGH, 500);
    _sequences[3].addFrame(LOW, 500);
  }

  _sequences.push_back(Sequence(ChangeOperationModeConfirmation));
  _sequences[3].addFrame(HIGH, 2000);
  _sequences[3].addFrame(LOW, 100);
}

void StatusLEDController::start(uint8_t sequenceType)
{
  _isRunning = false;
  _startTimeStamp = millis();
  int index = 0;
  for (Sequence s : _sequences)
  {
    if (s.getType() == sequenceType)
    {
      _isRunning = true;
      _sequenceIndex = index;
      Serial.println("Select sequence: " + String(_sequenceIndex));
      _sequences[_sequenceIndex].first();
      return;
    }
    index++;
  }
}

void StatusLEDController::stop()
{
  _isRunning = false;
  Serial.println("LEDSequence[stopped]");
}

void StatusLEDController::loop()
{
  if (!_isRunning)
    return;

  unsigned long diff = millis() - _startTimeStamp;
  //Serial.println("Execute sequence: " + String(_sequenceIndex));
  SequenceFrame frame = _sequences[_sequenceIndex].getCurrent();

  if (diff <= frame._duration)
  {
    if (_invert)
    {
      digitalWrite(_pin, !frame._status);
    }
    else
    {
      digitalWrite(_pin, frame._status);
    }
  }
  else
  {
    if (_sequences[_sequenceIndex].next())
    {
      _startTimeStamp = millis();
    }
    else
    {
      stop();
    }
  }
}
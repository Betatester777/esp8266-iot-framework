#include <SMA/SMAModbusSlave.h>
#include <ConnectionStateMachine.h>

SMAModbusSlave::SMAModbusSlave(String serverHost, uint16_t serverPort, uint16_t startAddress, uint16_t quantityOfRegisters, void (*onValueChanged)(uint32_t oldValue, uint32_t newValue))
{
  _transactionId = 1;
  _protocolId = 0;
  _functionCode = 3;
  _unitId = 3;
  _serverHost = serverHost;
  _serverPort = serverPort;
  _startAddress = startAddress;
  _quantityOfRegisters = quantityOfRegisters;
  _onValueChanged = onValueChanged;
  _requestPayload = new SMARequest(_transactionId, _protocolId, 6, _unitId, _functionCode, _startAddress, _quantityOfRegisters);
}

bool SMAModbusSlave::isTimerExpired()
{
  if (_delay > 0 && millis() > _timer + _delay)
  {
    _delay = 0;
    return true;
  }
  return false;
}

void SMAModbusSlave::startTimer(unsigned long delay)
{
  if (_delay == 0)
  {
    _delay = delay;
    _timer = millis();
    _isTimerStarted = true;
    Serial.println("start timer for MODBUS get value: " + String(delay));
  }
}

void SMAModbusSlave::stopTimer()
{
  _delay = 0;
}

void SMAModbusSlave::setHostAndPort(String serverHost, uint16_t serverPort)
{
  _serverHost = serverHost;
  _serverPort = serverPort;
}

int SMAModbusSlave::readRegister()
{
  WiFiClient client;
  int32_t newValue = 0;
  //client.setTimeout(10000);
  int retval = client.connect(_serverHost, _serverPort);
  if (retval != 1)
  {
    Serial.println(F("Connection failed."));
    connectionStateMachine.trigger(TRIGGER_MODBUS_CONNECTION_FAIL);
    return 1;
  }

  Serial.println("Connected to host: " + _serverHost + ":" + String(_serverPort));

  char requestBytes[12];
  _requestPayload->getBytes(requestBytes, sizeof(requestBytes));
  client.write(requestBytes, sizeof(requestBytes));
  Serial.print("sending buffer: \n" + _requestPayload->toString());
  unsigned long timeout = millis();
  while (client.available() < 11)
  {
    delay(1); // important to service the tcp stack
    if ((millis() - timeout) > 5000)
    {
      Serial.println("MODBUS client timeout !");
      connectionStateMachine.trigger(TRIGGER_MODBUS_CONNECTION_FAIL);
      client.stop();
      return 2;
    }
  }

  // Read all the lines of the reply from server and print them to Serial
  int bytesToReceive = client.available();
  std::vector<unsigned char> result;
  Serial.println("receiving...");

  for (int a = 0; a < bytesToReceive; a++)
  {
    result.push_back(client.read());
  }

  unsigned char *resultBuffer = &result[0];
  SMAResponse *response = new SMAResponse(resultBuffer, sizeof(resultBuffer));
  Serial.println("received buffer: \n" + response->toString());
  newValue = response->getS32Value();
  Serial.println("received MODBUS value: " + String(newValue));

  connectionStateMachine.trigger(TRIGGER_MODBUS_CONNECTION_SUCCESS);

  if (newValue != (int32_t)_registerValue)
  {
    this->_onValueChanged(this->_registerValue, newValue);
    this->_registerValue = newValue;
  }
  return 0;
}

String SMAModbusSlave::runTest()
{
  String JSON;
  StaticJsonDocument<1024> jsonBuffer;
  jsonBuffer["serverTestIsComplete"] = 0;
  jsonBuffer["serverTestConnectionStatus"] = 0;
  jsonBuffer["serverTestRequestStatus"] = 0;
  jsonBuffer["serverTestResponseValue"] = -1;

  WiFiClient client;

  Serial.println("Run connection test [host=" + _serverHost + ", port=" + String(_serverPort) + "]");

  int retval = client.connect(_serverHost, _serverPort);
  if (retval != 1)
  {
    Serial.println(F("Connection failed."));
    serializeJson(jsonBuffer, JSON);
    return JSON;
  }
  else
  {
    jsonBuffer["serverTestConnectionStatus"] = 1;
  }

  Serial.println("Connected to host: " + _serverHost + ":" + String(_serverPort));

  char requestBytes[12];
  _requestPayload->getBytes(requestBytes, sizeof(requestBytes));
  client.write(requestBytes, sizeof(requestBytes));
  Serial.print("sending: \n" + _requestPayload->toString());
  unsigned long timeout = millis();
  while (client.available() < 11)
  {
    delay(1); // important to service the tcp stack
    if ((millis() - timeout) > 5000)
    {
      Serial.println(">>> Client Timeout !");
      client.stop();
      serializeJson(jsonBuffer, JSON);
      return JSON;
    }
  }

  // Read all the lines of the reply from server and print them to Serial
  int bytesToReceive = client.available();
  std::vector<unsigned char> result;
  Serial.println("receiving...");

  for (int a = 0; a < bytesToReceive; a++)
  {
    result.push_back(client.read());
  }

  Serial.println("Result: ");

  unsigned char *resultBuffer = &result[0];
  SMAResponse *response = new SMAResponse(resultBuffer, sizeof(resultBuffer));
  Serial.println("received: \n" + response->toString());

  jsonBuffer["serverTestRequestStatus"] = 1;
  jsonBuffer["serverTestResponseValue"] = response->getS32Value();

  serializeJson(jsonBuffer, JSON);
  Serial.println(JSON);
  return JSON;
}

SMAModbusSlave *smaModbusSlave;

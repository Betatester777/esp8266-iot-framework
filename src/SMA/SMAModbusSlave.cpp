#include <SMA/SMAModbusSlave.h>
bool SMAModbusSlave::doConnect = false;

SMAModbusSlave::SMAModbusSlave(String serverHost, uint16_t serverPort, uint32_t requestInterval, uint16_t startAddress, uint16_t quantityOfRegisters, void (*onValueChanged)(uint32_t oldValue, uint32_t newValue))
{
  _transactionId = 1;
  _protocolId = 0;
  _functionCode = 3;
  _unitId = 3;
  _serverHost = serverHost;
  _serverPort = serverPort;
  _requestInterval = requestInterval;
  _startAddress = startAddress;
  _quantityOfRegisters = quantityOfRegisters;
  _onValueChanged = onValueChanged;
  _requestPayload = new SMARequest(_transactionId, _protocolId, 6, _unitId, _functionCode, _startAddress, _quantityOfRegisters);
  setRequestInterval(_requestInterval);
}

void SMAModbusSlave::setHostAndPort(String serverHost, uint16_t serverPort){
  _serverHost = serverHost;
  _serverPort = serverPort;
}

void SMAModbusSlave::setRequestInterval(uint32_t requestInterval)
{
  _requestInterval = requestInterval;
}

bool SMAModbusSlave::getIsEnabled()
{
  return _requestInterval > 0;
}

int SMAModbusSlave::_readRegister()
{
  WiFiClient client;
  int32_t newValue = 0;
  //client.setTimeout(10000);
  int retval = client.connect(_serverHost, _serverPort);
  if (retval != 1)
  {
    Serial.println(F("Connection failed."));
    return -1;
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
      return -1;
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
  newValue = response->getS32Value();
  Serial.println(newValue);

  if (newValue != (int32_t)_registerValue)
  {
    this->_onValueChanged(this->_registerValue, newValue);
    this->_registerValue = newValue;
  }
  return 1;
}

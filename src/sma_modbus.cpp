#include "sma_modbus.h"

SMAModbusSlave::SMAModbusSlave(String _inverterIp, uint32_t _requestInterval, void (*_onDataReceived)(uint32_t watts))
{
  client = new WiFiClient();
  requestValueTicker = new Ticker();

  setInverterIp(_inverterIp);
  setRequestInterval(_requestInterval);
  setOnDataReceived(_onDataReceived);
  outputPower=0;
}

const char SMAModbusSlave::currentOutputPowerRequestPayload[12]={0, 1, 0, 0, 0, 6, 0x7e, 4, 0x9d, 7, 0, 1};

void SMAModbusSlave::setInverterIp(String _inverterIp){
  inverterIp=_inverterIp;
}

void SMAModbusSlave::setRequestInterval(uint32_t _requestInterval){
  requestInterval=_requestInterval;
  requestValueTicker->detach();
  if(onDataReceived!=NULL && requestInterval>0){
    requestValueTicker->attach_ms(requestInterval, [this](){ this->sendRequest(); });
  }
}

void SMAModbusSlave::setOnDataReceived(void (*_onDataReceived)(uint32_t watts)){
  onDataReceived=_onDataReceived;
  requestValueTicker->detach();
  if(onDataReceived!=NULL && requestInterval>0){
    requestValueTicker->attach_ms(requestInterval, [this](){ this->sendRequest(); });
  }
}

int SMAModbusSlave::sendRequest()
{

  if (!client->connect(inverterIp, INVERTER_PORT))
  { //MODBUS port is always 502
    Serial.println("connection failed");
    return -1;
  }

  client->write(&currentOutputPowerRequestPayload[0], sizeof(currentOutputPowerRequestPayload));
  Serial.print("sending....");
  for (unsigned int i = 0; i < sizeof(currentOutputPowerRequestPayload); i++)
  {
    Serial.print(currentOutputPowerRequestPayload[i], HEX);
  }

  return getData();
}

int SMAModbusSlave::getData()
{

  unsigned long timeout = millis();
  while (client->available() < 11)
  {
    delay(1); // important to service the tcp stack
    if (millis() - timeout > 5000)
    {
      Serial.println(">>> Client Timeout !");
      client->stop();
      return -1;
    }
  }

  // Read all the lines of the reply from server and print them to Serial
  uint16_t watts = 0;
  int bytesToReceive = client->available();
  /*
    Response 11 bytes:
    00 01 (Transaction id)
    00 00 (Protocol ID)
    00 05 (length 5)
    7e (unit ID 126)
    04 (Function code: read register)
    02 (byte count 2)
    00 eb ( Data / Power in 10W, e.g. (235 *10 Watts))
  */
  uint16_t high, low;
  Serial.print(" >>  receiving: ");
  ;
  for (int a = 0; a < bytesToReceive - 2; a++)
  {
    Serial.print(client->read(), HEX);
    Serial.print(" ");
  }
  high = client->read();
  low = client->read();
  watts = 10 * ((high << 8) + low);


  Serial.print(String(watts) + " W");
  outputPower=watts;
  if(onDataReceived!=NULL){
    onDataReceived(watts);
  }
  return 0;
}

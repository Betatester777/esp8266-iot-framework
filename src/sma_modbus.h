#ifndef __SMA_MODBUS_H__
#define __SMA_MODBUS_H__

#include <Ticker.h>
#include <ESP8266WiFi.h>

#define INVERTER_PORT 502

class SMAModbusSlave
{

public:
  SMAModbusSlave(String inverterIp, uint32_t _requestInterval, void (*_onDataReceived)(uint32_t watts));
  ~SMAModbusSlave();
  void setInverterIp(String _inverterIp);
  void setRequestInterval(uint32_t _requestInterval);
  void setOnDataReceived(void (*_onDataReceived)(uint32_t watts));

private:
  WiFiClient *client;
  Ticker *requestValueTicker;
  void (*onDataReceived)(uint32_t watts);

    /*
  get SMA STPx000TL Modbus ("sunspec") datasheet here: https://www.sma.de/en/products/solarinverters/sunny-tripower-5000tl-12000tl.html
  Request 12 bytes:
  00 01 (Transaction id)
  00 00 (Protocol ID)
  00 06 (bytes following / length: 6)
  7e (unit ID 126 / is always 126 no matter what you configured)
  04 (Function code: read register)
  9d 07 (register 40199 (actual power)) note: according to datasheet: use register number-1 (40200-1)
  00 01  (words to read: 0001)
*/
  static const char currentOutputPowerRequestPayload[12];
  String inverterIp;
  uint32_t requestInterval;
  uint32_t outputPower;
  int  sendRequest();
  int  getData();
};

extern SMAModbusSlave smaModbusSlave;

#endif
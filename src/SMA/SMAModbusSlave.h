#ifndef __SMA_MODBUS_SLAVE_H__
#define __SMA_MODBUS_SLAVE_H__

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <SMA/SMARequest.h>
#include <SMA/SMAResponse.h>
#include "ArduinoJson.h"

class SMAModbusSlave
{
public:
  SMAModbusSlave(String serverHost, uint16_t serverPort, uint16_t startAddress, uint16_t quantityOfRegisters, void (*onValueChanged)(uint32_t oldValue, uint32_t newValue));
  ~SMAModbusSlave();
  void setHostAndPort(String serverHost, uint16_t serverPort);
  int getValue();
  static bool doConnect;
  int readRegister();
  String runTest();

  bool isTimerExpired();
  void startTimer(unsigned long delay);
  void stopTimer();

private:
  WiFiClient *client;
  void (*_onValueChanged)(uint32_t oldValue, uint32_t newValue);
  SMARequest *_requestPayload;
  uint32_t _transactionId;
  uint32_t _protocolId;
  uint16_t _functionCode;
  uint16_t _unitId;
  String _serverHost;
  uint16_t _serverPort;
  uint32_t _requestInterval;
  uint16_t _startAddress;
  uint16_t _quantityOfRegisters;
  uint32_t _registerValue;
  bool _isTimerStarted;
  unsigned long _delay;
  unsigned long _timer;
};

extern SMAModbusSlave *smaModbusSlave;

#endif
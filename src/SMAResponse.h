#ifndef __SMA_RESPONSE_H__
#define __SMA_RESPONSE_H__

#include <Arduino.h>

class SMAResponse
{
public:
  SMAResponse(unsigned char *, int);
  ~SMAResponse();

  String toString();
  uint16_t getU16Value();
  int16_t getS16Value();
  uint32_t getU32Value();
  int32_t getS32Value();

private:
  uint16_t _transaction_id;
  uint16_t _protocol_id;
  uint16_t _bytes_following;
  uint16_t _unit_id;
  uint8_t _function_code;
  uint8_t _byte_count;
  std::vector<unsigned char> _bytes;
};

#endif
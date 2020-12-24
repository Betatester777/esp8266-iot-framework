#ifndef __SMA_REQUEST_H__
#define __SMA_REQUEST_H__

#include <Arduino.h>

class SMARequest
{
public:
  SMARequest(uint16_t transaction_id,
             uint16_t protocol_id,
             uint16_t bytes_following,
             uint16_t unit_id,
             uint8_t function_code,
             uint16_t register_addtess,
             uint16_t words_to_read);
  ~SMARequest();  
  String toString();
  void setTransactionId(uint16_t transaction_id);
  void getBytes(char *, int);
private:
  uint16_t _transaction_id;
  uint16_t _protocol_id;
  uint16_t _bytes_following;
  uint16_t _unit_id;
  uint8_t _function_code;
  uint16_t _register_addtess;
  uint16_t _words_to_read;
};

#endif
#include <SMA/SMAResponse.h>

SMAResponse::SMAResponse(unsigned char *buffer, int size)
{
  _transaction_id = (buffer[0] >> 8) + buffer[1];
  _protocol_id = (buffer[2] >> 8) + buffer[3];
  _bytes_following = (buffer[4] >> 8) + buffer[5];
  _unit_id = buffer[6];
  _function_code = buffer[7];
  _byte_count = buffer[8];
  for (int byteIndex = 0; byteIndex < _byte_count; byteIndex++)
  {
    _bytes.push_back(buffer[9 + byteIndex]);
  }
}

String SMAResponse::toString()
{
  String result = "response:{";
  result += "transaction_id:" + String(_transaction_id) + ",";
  result += "protocol_id:" + String(_protocol_id) + ",";
  result += "bytes_following:" + String(_bytes_following) + ",";
  result += "unit_id:" + String(_unit_id) + ",";
  result += "function_code:" + String(_function_code) + ",";
  result += "byte_count: " + String(_byte_count) + ",";
  result += "bytes:";
  for (unsigned char byte : _bytes)
  {
    result += "" + String((uint8_t)byte) + " ";
  }
  result += "}";
  return result;}

uint16_t SMAResponse::getU16Value()
{
  uint16_t value = 0;
    if(_bytes[0]==0xFF && _bytes[1]==0xFF){
    return 0;
  }
  for (int byteIndex = _bytes.size() - 1; byteIndex >= 0; byteIndex--)
  {
    value += ((uint16_t)_bytes[byteIndex]) << (8 * (3 - byteIndex));
  }
  return value;
}

int16_t SMAResponse::getS16Value()
{
  int16_t value = 0;

  if(_bytes[0]==0x80 && _bytes[1]==0x00){
    return 0;
  }

  for (int byteIndex = _bytes.size() - 1; byteIndex >= 0; byteIndex--)
  {
    value += ((int16_t)_bytes[byteIndex]) << (8 * (3 - byteIndex));
  }
  return value;
}

uint32_t SMAResponse::getU32Value()
{
  uint32_t value = 0;

  if(_bytes[0]==0xFF && _bytes[1]==0xFF & _bytes[2]==0xFF  && _bytes[3]==0xFF ){
    return 0;
  }

  for (int byteIndex = _bytes.size() - 1; byteIndex >= 0; byteIndex--)
  {
    value += ((uint32_t)_bytes[byteIndex]) << (8 * (3 - byteIndex));
  }
  return value;
}

int32_t SMAResponse::getS32Value()
{
  int32_t value = 0;

  if(_bytes[0]==0x80 && _bytes[1]==0x00 & _bytes[2]==0x00  && _bytes[3]==0x00 ){
    return 0;
  }

  for (int byteIndex = _bytes.size() - 1; byteIndex >= 0; byteIndex--)
  {
    value += ((int32_t)_bytes[byteIndex]) << (8 * (3 - byteIndex));
  }
  return value;
}
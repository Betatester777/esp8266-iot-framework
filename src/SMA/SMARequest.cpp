#include <SMA/SMARequest.h>

SMARequest::SMARequest(uint16_t transaction_id,
                       uint16_t protocol_id,
                       uint16_t bytes_following,
                       uint16_t unit_id,
                       uint8_t function_code,
                       uint16_t register_addtess,
                       uint16_t words_to_read)
{
  _transaction_id = transaction_id;
  _protocol_id = protocol_id;
  _bytes_following = bytes_following;
  _unit_id = unit_id;
  _function_code = unit_id;
  _register_addtess = register_addtess;
  _words_to_read = words_to_read;
}

String SMARequest::toString()
{
  String result = "request:{";
  result += "transaction_id: " + String(_transaction_id) + ", ";
  result += "protocol_id: " + String(_protocol_id) + ", ";
  result += "bytes_following: " + String(_bytes_following) + ", ";
  result += "unit_id: " + String(_unit_id) + ", ";
  result += "function_code: " + String(_function_code) + ", ";
  result += "register_addtess: " + String(_register_addtess) + ", ";
  result += "words_to_read: " + String(_words_to_read) + "}";
  return result;
}

void SMARequest::setTransactionId(uint16_t transaction_id)
{
  _transaction_id = transaction_id;
}

void SMARequest::getBytes(char *payloadBuffer, int size)
{
  std::vector<unsigned char> result;
  result.push_back(_transaction_id >> 8);
  result.push_back(_transaction_id);

  result.push_back(_protocol_id >> 8);
  result.push_back(_protocol_id);

  result.push_back(_bytes_following >> 8);
  result.push_back(_bytes_following);

  result.push_back(_unit_id);

  result.push_back(_function_code);

  result.push_back(_register_addtess >> 8);
  result.push_back(_register_addtess);

  result.push_back(_words_to_read >> 8);
  result.push_back(_words_to_read);

  int byteIndex = 0;
  for (unsigned char byte : result)
  {
    payloadBuffer[byteIndex] = byte;
    byteIndex++;
  }
}

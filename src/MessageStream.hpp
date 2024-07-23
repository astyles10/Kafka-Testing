#ifndef MESSAGE_STREAM_HPP
#define MESSAGE_STREAM_HPP

#include "IOQueue.cpp"
#include "Messages/GenericMessage.hpp"

/* 
  MessageStream is responsible for converting an input stream into a GenericMessage output stream
 */

class MessageStream {
 public:
  MessageStream();
  ~MessageStream();
 private:
};

#endif
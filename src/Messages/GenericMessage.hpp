#ifndef STREAM_DATA_HPP
#define STREAM_DATA_HPP

#include <string>

/* 
  GenericMessage: Generic data structure.
  Provides interface for accessing any type data format such as JSON, XML, etc.
 */

class GenericMessage {
 public:
  GenericMessage() = default;
  virtual ~GenericMessage() = default;

  virtual std::string Get() { return ""; }
};

#endif
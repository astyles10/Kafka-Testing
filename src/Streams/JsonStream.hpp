#ifndef JSON_STREAM_HPP
#define JSON_STREAM_HPP

#include "InputStream.hpp"

#include <nlohmann/json.hpp>

class JsonStream : public InputStream {
 public:
  JsonStream(std::basic_ios<char>& inIoStream);
 private:
  void Consume();
};

#endif

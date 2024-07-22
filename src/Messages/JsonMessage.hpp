#ifndef JSON_STREAM_HPP
#define JSON_STREAM_HPP

#include <nlohmann/json.hpp>

#include "GenericMessage.hpp"

using json = nlohmann::json;

class JsonMessage : public GenericMessage {
 public:
  JsonMessage() = default;
  JsonMessage(const json& inJson) : fData(inJson) {};
  virtual ~JsonMessage() = default;

  std::string Get() {
    return fData.dump();
  }
 private:
  json fData;
};

#endif
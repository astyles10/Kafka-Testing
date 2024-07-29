#ifndef JSON_STREAM_HPP
#define JSON_STREAM_HPP

#include <nlohmann/json.hpp>

#include "GenericMessage.hpp"

using json = nlohmann::json;

class JsonMessage : public GenericMessage {
 public:
  JsonMessage() = default;
  JsonMessage(std::basic_ios<char>& inStream) {
    Consume(inStream);
  };
  JsonMessage(const json& inJson) : fData(inJson) {};
  virtual ~JsonMessage() = default;

  const std::string Get() const override {
    return fData.dump();
  }

 private:
  void Consume(std::basic_ios<char>& inStream) override {
    std::cout << "JsonMessage consume called\n";
    std::istream aStream(inStream.rdbuf());
    fData = json::parse(aStream, nullptr, true, true);
  }

  json fData;
};

#endif
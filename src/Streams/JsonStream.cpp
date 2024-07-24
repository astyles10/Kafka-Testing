#include "JsonStream.hpp"
#include "Messages/JsonMessage.hpp"



JsonStream::JsonStream(std::basic_ios<char>& inIoStream) : InputStream(inIoStream) {
}

void JsonStream::Consume() {
  nlohmann::json aJson;
  aJson.parse(fIoStream);
}

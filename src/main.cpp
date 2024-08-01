
#include <csignal>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <rdkafkacpp.h>

#include "IOQueue.hpp"
#include "Messages/JsonMessage.hpp"
#include "KafkaStream.hpp"

// Reference: https://github.com/confluentinc/librdkafka/blob/master/examples/producer.cpp

using json = nlohmann::json;

json LoadTestDataFile(const std::string& inFileName) {
  json aTestData;
  std::ifstream aTestFileStream(inFileName);
  return json::parse(aTestFileStream);
}

int main(int argc, char** argv) {
  // TODO: Implement command line parameters to initialise a stream
  try {
    const json aConfigFile = LoadTestDataFile("./config.json");
    std::cout << "Read config file " << aConfigFile.dump() << std::endl;
    KafkaStreamConfig aKafkaConfig(aConfigFile);
  } catch (std::exception& e) {
    std::cerr << "Failed to read config: " << e.what() << std::endl;
  }
  // std::shared_ptr<InputStream<JsonMessage>> aStreamPtr = std::make_shared<InputStream<JsonMessage>>();
  // std::shared_ptr<KafkaStream> aKafkaStream = std::make_shared<KafkaStream>(aConfigFile);
  // aStreamPtr->PushObserver(aKafkaStream);

  return 0;
}
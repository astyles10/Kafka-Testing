
#include <csignal>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <rdkafkacpp.h>
#include <unistd.h>

#include "IOQueue.hpp"
#include "Messages/JsonMessage.hpp"
#include "KafkaStream.hpp"
#include "Kafka/BasicDeliveryReportCallback.hpp"

// Reference: https://github.com/confluentinc/librdkafka/blob/master/examples/producer.cpp

std::atomic_bool gStreamRunning = false;
int32_t gMessageThrottleMs = 100;

std::shared_ptr<BasicDeliveryReportCb> gCallbackPtr;
std::shared_ptr<KafkaProducerConfig> gKafkaConfig;
std::shared_ptr<KafkaStream> gKafkaStream;

std::shared_ptr<InputStream<JsonMessage>> gUserDataStream;
json gUserData;

void InitialiseSignals();
void InitialiseKafka();
void InitialiseInputStream();
json LoadTestDataFile(const std::string& inFileName);
void StreamTestDataToKafka();
void HandleSignal(int inSignal);

int main(int argc, char** argv) {
  // TODO: Implement command line parameters to initialise a stream
  // See boost program options

  InitialiseSignals();
  // Obviously these try catches littering the main function are not nice to look at,
  // Move them into their own functions
  try {
    InitialiseKafka();
  } catch (std::exception& e) {
    std::cerr << "Failed to initialise kafka: " << e.what() << std::endl;
  }

  try {
    InitialiseInputStream();
  } catch (std::exception& e) {
    std::cerr << "Failed to initialise input stream: " << e.what() << "\n";
  }

  StreamTestDataToKafka();
  while (gStreamRunning) {

  }
  std::cout << "Program complete\n";
  return 0;
}

void InitialiseKafka() {
  const json aConfigFile = LoadTestDataFile("./config.json");
  gCallbackPtr = std::make_shared<BasicDeliveryReportCb>(BasicDeliveryReportCb());
  gKafkaConfig = std::make_shared<KafkaProducerConfig>(aConfigFile, gCallbackPtr);
  gKafkaStream = std::make_shared<KafkaStream>(gKafkaConfig);
  gStreamRunning = true;
}

void InitialiseInputStream() {
  gUserDataStream = std::make_shared<InputStream<JsonMessage>>();
  gUserDataStream->PushObserver(gKafkaStream);
  gUserData = LoadTestDataFile("./users.json");
}

json LoadTestDataFile(const std::string& inFileName) {
  json aTestData;
  std::ifstream aTestFileStream(inFileName);
  return json::parse(aTestFileStream);
}

void StreamTestDataToKafka() {
  const json& aUsers = gUserData["/users"_json_pointer];
  const std::size_t aNumberOfUsers = aUsers.size();
  std::size_t aUserIndex = 0;
  while (aUserIndex < 5) {
    const json& aUser = aUsers.at(aUserIndex);
    JsonMessage aMessage(aUser);
    gUserDataStream->Commit(aMessage);
    ++aUserIndex;
  }
}

void InitialiseSignals() {
  signal(SIGINT, HandleSignal);
  signal(SIGTERM, HandleSignal);
}

void HandleSignal(int inSignal) {
  gStreamRunning = false;
}

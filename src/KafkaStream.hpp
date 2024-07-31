#ifndef KAFKA_STREAM_HPP
#define KAFKA_STREAM_HPP

#include "Observer.hpp"
#include "Streams/InputStream.hpp"

#include <iostream>
#include <nlohmann/json.hpp>
#include <rdkafkacpp.h>

using json = nlohmann::json;

struct KafkaStreamConfig {
  KafkaStreamConfig(const json& inConfig) {
    fServerAddress = inConfig.value<std::string>("/kafka/serverAddress"_json_pointer, "");
    fPort = inConfig.value<uint16_t>("/kafka/port"_json_pointer, 0);
    fTopic = inConfig.value<std::string>("/kafka/topic"_json_pointer, "");
  }
  ~KafkaStreamConfig() = default;
  std::string fServerAddress;
  uint16_t fPort;
  std::string fTopic;
};

// TODO: Do something with this
class ExampleDeliveryReportCb : public RdKafka::DeliveryReportCb {
 public:
  void dr_cb(RdKafka::Message& inMessage) {
    if (inMessage.err()) {
      std::cerr << "!! Message delivery failed: " << inMessage.errstr() << "\n";
      return;
    }
    std::cout << "Message delivered to topic " << inMessage.topic_name() << " [" << inMessage.partition() << "] at offset " << inMessage.offset() << "\n";
  }
};

class KafkaStream : public Observer {
 public:
  KafkaStream(const json& inConfig);
  ~KafkaStream();
  bool Start();
  void Stop();

  void Notify(const GenericMessage& inMessage) override;

 private:
  bool InitialiseConfig();
  bool InitialiseStream();

  const json fConfigFile;
  std::unique_ptr<RdKafka::Conf> fConfig;
  ExampleDeliveryReportCb fDeliveryCallback;
  std::unique_ptr<RdKafka::Producer> fProducer;
};

#endif
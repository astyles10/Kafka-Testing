#ifndef KAFKA_STREAM_HPP
#define KAFKA_STREAM_HPP

#include "Observer.hpp"
#include "Streams/InputStream.hpp"

#include <iostream>
#include <nlohmann/json.hpp>
#include <queue>
#include <rdkafkacpp.h>

using json = nlohmann::json;

struct KafkaStreamConfig {
  KafkaStreamConfig(const json& inConfig) {
    fServerAddress = inConfig.at("/kafka/serverAddress"_json_pointer).get<std::string>();
    fPort = inConfig.at("/kafka/port"_json_pointer).get<uint16_t>();
    fTopic = inConfig.at("/kafka/topic"_json_pointer).get<std::string>();
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

  void Notify(std::shared_ptr<GenericMessage> inMessage) override;

 private:
  void ProducerLoop();
  void ProduceToStream();
  void HandleProduceErrorCode(const RdKafka::ErrorCode& inErrorCode) const;
  void HandleRetry(const RdKafka::ErrorCode& inErrorCode);
  bool DetermineIfRetryProduce(const RdKafka::ErrorCode& inErrorCode) const;
  void FlushMessageQueue();
  void InitialiseConfig(const json& inConfig);
  void InitialiseProducer();

  std::atomic_int32_t fPollTimeoutMs;
  std::unique_ptr<std::thread> fPollThread;
  std::queue<std::shared_ptr<GenericMessage>> fMessageBacklog;
  std::unique_ptr<RdKafka::Conf> fKafkaConfig;
  ExampleDeliveryReportCb fDeliveryCallback;
  std::unique_ptr<RdKafka::Producer> fProducer;
  std::string fTopic;
};

#endif
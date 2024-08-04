#ifndef KAFKA_STREAM_HPP
#define KAFKA_STREAM_HPP

#include "Observer.hpp"
#include "Streams/InputStream.hpp"

#include <iostream>
#include <nlohmann/json.hpp>
#include <queue>
#include <rdkafkacpp.h>

using json = nlohmann::json;

class KafkaProducerConfig {
 public:
  KafkaProducerConfig(const json& inConfig, std::shared_ptr<RdKafka::DeliveryReportCb> inReportCallback) : fDeliveryReportCallback(inReportCallback) {
    DetermineSettings(inConfig);
    InitialiseConfig();
  }
  ~KafkaProducerConfig() = default;

  // TODO: If multiple streams are open this needs to be a shared pointer
  std::unique_ptr<RdKafka::Conf> GetConfig() {
    return std::move(fKafkaConfig);
  }
 private:
  void DetermineSettings(const json& inConfig) {
    fServerAddress = inConfig.at("/kafka/serverAddress"_json_pointer).get<std::string>();
    fPort = inConfig.at("/kafka/port"_json_pointer).get<uint16_t>();
    fTopic = inConfig.at("/kafka/topic"_json_pointer).get<std::string>();
  }
  void InitialiseConfig() {
    fKafkaConfig = std::unique_ptr<RdKafka::Conf>(RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL));
    SetServerAddress();
    SetCallback();
  }

  void SetServerAddress() {
    std::string aError;
    if (fKafkaConfig->set("bootstrap.servers", fServerAddress, aError) != RdKafka::Conf::CONF_OK) {
      aError = "Failed to set config: " + aError;
      throw std::runtime_error(aError);
    }
  }

  void SetCallback() {
    std::string aError;
    if (fKafkaConfig->set("dr_cb", fDeliveryReportCallback.get(), aError) != RdKafka::Conf::CONF_OK) {
      std::cerr << aError << "\n";
      throw std::runtime_error(aError);
    }
  }

  std::unique_ptr<RdKafka::Conf> fKafkaConfig;
  std::shared_ptr<RdKafka::DeliveryReportCb> fDeliveryReportCallback;
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
#ifndef KAFKA_PRODUCER_CONFIG
#define KAFKA_PRODUCER_CONFIG

#include <nlohmann/json.hpp>
#include <rdkafkacpp.h>
#include <iostream>

using json = nlohmann::json;

class KafkaProducerConfig {
 public:
  KafkaProducerConfig(const json& inConfig, std::shared_ptr<RdKafka::DeliveryReportCb> inDeliveryReportCallback);
  ~KafkaProducerConfig() = default;

  std::unique_ptr<RdKafka::Conf> ConsumeConfig();
  const std::string& GetServerAddress() const;
  const uint16_t& GetPort() const;
  const std::string& GetTopic() const;

 private:
  void DetermineSettings(const json& inConfig);
  void InitialiseConfig();
  void SetServerAddress();
  void SetDeliveryReportCallback();

  std::unique_ptr<RdKafka::Conf> fKafkaConfig;
  std::shared_ptr<RdKafka::DeliveryReportCb> fDeliveryReportCallback;
  std::string fServerAddress;
  uint16_t fPort;
  std::string fTopic;
};

#endif

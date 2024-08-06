#include "KafkaProducerConfig.hpp"

KafkaProducerConfig::KafkaProducerConfig(const json& inConfig, std::shared_ptr<RdKafka::DeliveryReportCb> inDeliveryReportCallback): fDeliveryReportCallback(inDeliveryReportCallback) {
  DetermineSettings(inConfig);
  InitialiseConfig();
}

std::unique_ptr<RdKafka::Conf> KafkaProducerConfig::ConsumeConfig() {
  return std::move(fKafkaConfig);
}

void KafkaProducerConfig::DetermineSettings(const json& inConfig) {
  fServerAddress = inConfig.at("/kafka/serverAddress"_json_pointer).get<std::string>();
  fPort = inConfig.at("/kafka/port"_json_pointer).get<uint16_t>();
  fTopic = inConfig.at("/kafka/topic"_json_pointer).get<std::string>();
}

void KafkaProducerConfig::InitialiseConfig() {
  RdKafka::Conf* aConfigPtr = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
  // fKafkaConfig.reset(aConfigPtr);
  SetServerAddress();
  SetDeliveryReportCallback();
}

void KafkaProducerConfig::SetServerAddress() {
  std::string aError;
  if (fKafkaConfig->set("bootstrap.servers", fServerAddress, aError) != RdKafka::Conf::CONF_OK) {
    aError = "Failed to set config: " + aError;
    throw std::runtime_error(aError);
  }
}

void KafkaProducerConfig::SetDeliveryReportCallback() {
  std::string aError;
  if (fKafkaConfig->set("dr_cb", fDeliveryReportCallback.get(), aError) != RdKafka::Conf::CONF_OK) {
    std::cerr << aError << "\n";
    throw std::runtime_error(aError);
  }
}

const std::string& KafkaProducerConfig::GetServerAddress() const {
  return fServerAddress;
}

const uint16_t& KafkaProducerConfig::GetPort() const {
  return fPort;
}

const std::string& KafkaProducerConfig::GetTopic() const {
  return fTopic; 
}
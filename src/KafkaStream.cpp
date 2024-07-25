#include "KafkaStream.hpp"

KafkaStream::KafkaStream(const json& inConfig, InputStream<GenericMessage>& inInputStream) : fInputStream(inInputStream) {
  InitialiseConfig(inConfig);
  InitialiseStream();
}

KafkaStream::~KafkaStream() {
  Stop();
}

void KafkaStream::Start() {
  // This should start a message stream
  fIoQueue.Start();
}

void KafkaStream::Stop() {
  fIoQueue.Stop();
}

void KafkaStream::InitialiseConfig(const json& inConfig) {
  fConfig = std::unique_ptr<RdKafka::Conf>(RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL));

  // TODO: Figure out a clean solution to handling invalid config options
  std::string aError;
  if (fConfig->set("bootstrap.servers", inConfig.value<std::string>("/kafka/server"_json_pointer, ""), aError) != RdKafka::Conf::CONF_OK) {
    aError = "Failed to set config: " + aError;
    throw std::runtime_error(aError);
  }

  if (fConfig->set("dr_cb", &fDeliveryCallback, aError) != RdKafka::Conf::CONF_OK) {
    std::cerr << aError << "\n";
    throw std::runtime_error(aError);
  }
}

void KafkaStream::InitialiseStream() {
  std::string aError;
  fProducer = std::unique_ptr<RdKafka::Producer>(RdKafka::Producer::create(fConfig.get(), aError));
  if (!fProducer) {
    throw std::runtime_error(aError);
  }
}
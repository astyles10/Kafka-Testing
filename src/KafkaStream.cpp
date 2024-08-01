#include "KafkaStream.hpp"

KafkaStream::KafkaStream(const json& inConfig) : fPollTimeoutMs(0) {
  InitialiseConfig(inConfig);
  InitialiseProducer();
  
}

KafkaStream::~KafkaStream() {
  FlushMessageQueue();
}

void KafkaStream::FlushMessageQueue() {
  fProducer->flush(10 * 1000);
  if (fProducer->outq_len() > 0) {
    std::cerr << "% " << fProducer->outq_len() << " messages were not delivered" << std::endl;
  }
}

void KafkaStream::Notify(const std::shared_ptr<GenericMessage> inMessage) {
  fMessageBacklog.push(inMessage);
}

void KafkaStream::ProducerLoop() {
  fProducer->poll(fPollTimeoutMs);
  ProduceToStream();
}

void KafkaStream::ProduceToStream() {
  std::shared_ptr<GenericMessage> aMessagePtr = fMessageBacklog.front();
  // TODO: Should a KafkaConfig struct be made with members topic, server, port, etc?
  // What about Key, timestamp, messageHeaders etc?
  // What is the purpose of an 'opaque value'
  RdKafka::ErrorCode aErrorCode = fProducer->produce(
    "topic",
    RdKafka::Topic::PARTITION_UA, // Use default partition, is this desired?
    RdKafka::Producer::RK_MSG_COPY,
    const_cast<char*>(aMessagePtr->Get().c_str()),
    aMessagePtr->Get().size(),
    NULL, // Key
    0, // ??
    0, // Timestamp
    NULL, // Message headers
    NULL // Per message opaque value passed to delivery report
    );
  HandleProduceErrorCode(aErrorCode);
  HandleRetry(aErrorCode);
}

void KafkaStream::HandleProduceErrorCode(const RdKafka::ErrorCode& inErrorCode) const {
  if (inErrorCode != RdKafka::ERR_NO_ERROR) {
    std::cerr << "!! Failed to produce to topic " << "topic" << ": " << RdKafka::err2str;
  }
}

void KafkaStream::HandleRetry(const RdKafka::ErrorCode& inErrorCode) {
  bool aRetryProduceMessage = DetermineIfRetryProduce(inErrorCode);
  if (aRetryProduceMessage) {
    fPollTimeoutMs = 1000;
  } else {
    fMessageBacklog.pop();
    fPollTimeoutMs = 0;
  }
}

bool KafkaStream::DetermineIfRetryProduce(const RdKafka::ErrorCode& inErrorCode) const {
  return (inErrorCode == RdKafka::ERR__QUEUE_FULL);
}

void KafkaStream::InitialiseConfig(const json& inConfig) {
  fKafkaConfig = std::unique_ptr<RdKafka::Conf>(RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL));
  std::string aError;
  if (fKafkaConfig->set("bootstrap.servers", inConfig.value<std::string>("/kafka/server"_json_pointer, ""), aError) != RdKafka::Conf::CONF_OK) {
    aError = "Failed to set config: " + aError;
    throw std::runtime_error(aError);
  }

  if (fKafkaConfig->set("dr_cb", &fDeliveryCallback, aError) != RdKafka::Conf::CONF_OK) {
    std::cerr << aError << "\n";
    throw std::runtime_error(aError);
  }
}

void KafkaStream::InitialiseProducer() {
  std::string aError;
  fProducer = std::unique_ptr<RdKafka::Producer>(RdKafka::Producer::create(fKafkaConfig.get(), aError));
  if (!fProducer) {
    throw std::runtime_error(aError);
  }
}

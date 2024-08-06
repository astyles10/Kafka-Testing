#include "KafkaStream.hpp"

KafkaStream::KafkaStream(KafkaProducerConfig& inConfig) : fPollTimeoutMs(0) {
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
    fKafkaConfig->GetTopic(),
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

void KafkaStream::InitialiseProducer() {
  std::string aError;
  std::unique_ptr<RdKafka::Conf> aConfig = fKafkaConfig->ConsumeConfig();
  fProducer = std::unique_ptr<RdKafka::Producer>(RdKafka::Producer::create(aConfig.get(), aError));
  if (!fProducer) {
    throw std::runtime_error(aError);
  }
}

#include "KafkaStream.hpp"

KafkaStream::KafkaStream(std::shared_ptr<KafkaProducerConfig> inConfig) : fRunning(false), fKafkaConfig(inConfig), fPollTimeoutMs(0) {
  InitialiseProducer();
  InitialiseProducerThread();
}

KafkaStream::~KafkaStream() {
  CeaseProducerThread();
}

void KafkaStream::CeaseProducerThread() {
  FlushProducerQueue();
  fRunning = false;
  // TODO: Determine if there's any other tasks needed to clean up the producer/thread
  if (fProducerThread->joinable()) {
    fProducerThread->join();
  }
}

void KafkaStream::FlushProducerQueue() {
  fProducer->flush(10 * 1000);
  if (fProducer->outq_len() > 0) {
    std::cerr << "% " << fProducer->outq_len() << " messages were not delivered" << std::endl;
  }
}

void KafkaStream::Notify(const std::shared_ptr<GenericMessage> inMessage) {
  fMessageBacklog.push(inMessage);
}

void KafkaStream::ProducerLoop() {
  while (fRunning) {
    fProducer->poll(fPollTimeoutMs);
    ProcessMessageBacklog();
  }
}

void KafkaStream::ProcessMessageBacklog() {
  if (fMessageBacklog.size() > 0) {
    ProduceMessage();
  }
}

void KafkaStream::ProduceMessage() {
  std::shared_ptr<GenericMessage> aMessagePtr = fMessageBacklog.front();
  std::cout << "Producing message: " << aMessagePtr->Get() << std::endl;
  // TODO: Find out what else should be included in a kafka config
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
  bool aShouldRetry = false;
  switch (inErrorCode) {
    case RdKafka::ERR__QUEUE_FULL:
    case RdKafka::ERR_NETWORK_EXCEPTION:
    case RdKafka::ERR_BROKER_NOT_AVAILABLE:
      aShouldRetry = true;
    default:
      aShouldRetry = false;
  }
  return aShouldRetry;
}

void KafkaStream::InitialiseProducer() {
  std::string aError;
  fProducer = std::unique_ptr<RdKafka::Producer>(RdKafka::Producer::create(fKafkaConfig->ConsumeConfig(), aError));
  if (!fProducer) {
    throw std::runtime_error(aError);
  }
}

void KafkaStream::InitialiseProducerThread() {
  fRunning = true;
  fProducerThread = std::make_unique<std::thread>(std::bind(&KafkaStream::ProducerLoop, this));
}

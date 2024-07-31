#include "KafkaStream.hpp"

KafkaStream::KafkaStream(const json& inConfig) : fConfigFile(inConfig) {
}

KafkaStream::~KafkaStream() {
  Stop();
}

// TODO: Clean code - is it better to initialise the entire state in the constructor?
// How should errors be handled?
bool KafkaStream::Start() {
  InitialiseConfig();
  InitialiseStream();
}

void KafkaStream::Stop() {
  fProducer->flush(10 * 1000);
  if (fProducer->outq_len() > 0) {
    std::cerr << "% " << fProducer->outq_len() << " messages were not delivered" << std::endl;
  }
  fConfig.reset();
  fProducer.reset();
}

void KafkaStream::Notify(const GenericMessage& inMessage) {
  // TODO: Should a KafkaConfig struct be made with members topic, server, port, etc?
  RdKafka::ErrorCode aErrorCode = fProducer->produce(
    "topic",
    RdKafka::Topic::PARTITION_UA, // Use default partition, is this desired?
    RdKafka::Producer::RK_MSG_COPY,
    const_cast<char*>(inMessage.Get().c_str()),
    inMessage.Get().size(),
    NULL, // Key
    0, // ??
    0, // Timestamp
    NULL, // Message headers
    NULL // Per message opaque value passed to delivery report
    );
  
  if (aErrorCode != RdKafka::ERR_NO_ERROR) {
    std::cerr << "!! Failed to produce to topic " << "topic" << ": " << RdKafka::err2str;
    if (aErrorCode == RdKafka::ERR__QUEUE_FULL) {
      // Do a retry here
      // poll is a blocking call - this must be considered in the polling thread
      fProducer->poll(1000);
    }
  }

  // TODO: Polling should be handled in a separate thread to serve the delivery report queue
  int aEventsServed = fProducer->poll(0);
}

bool KafkaStream::InitialiseConfig() {
  fConfig = std::unique_ptr<RdKafka::Conf>(RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL));

  // TODO: Figure out a clean solution to handling invalid config options
  std::string aError;
  if (fConfig->set("bootstrap.servers", fConfigFile.value<std::string>("/kafka/server"_json_pointer, ""), aError) != RdKafka::Conf::CONF_OK) {
    aError = "Failed to set config: " + aError;
    throw std::runtime_error(aError);
    return false;
  }

  if (fConfig->set("dr_cb", &fDeliveryCallback, aError) != RdKafka::Conf::CONF_OK) {
    std::cerr << aError << "\n";
    throw std::runtime_error(aError);
    return false;
  }
  return true;
}

bool KafkaStream::InitialiseStream() {
  std::string aError;
  fProducer = std::unique_ptr<RdKafka::Producer>(RdKafka::Producer::create(fConfig.get(), aError));
  if (!fProducer) {
    throw std::runtime_error(aError);
    return false;
  }
  return true;
}
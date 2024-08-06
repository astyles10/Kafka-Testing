#ifndef KAFKA_STREAM_HPP
#define KAFKA_STREAM_HPP

#include "Observer.hpp"
#include "Kafka/KafkaProducerConfig.hpp"
#include "Streams/InputStream.hpp"

#include <nlohmann/json.hpp>
#include <queue>

using json = nlohmann::json;

class KafkaStream : public Observer {
 public:
  KafkaStream(KafkaProducerConfig& inConfig);
  ~KafkaStream();

  void Notify(std::shared_ptr<GenericMessage> inMessage) override;

 private:
  void ProducerLoop();
  void ProduceToStream();
  void HandleProduceErrorCode(const RdKafka::ErrorCode& inErrorCode) const;
  void HandleRetry(const RdKafka::ErrorCode& inErrorCode);
  bool DetermineIfRetryProduce(const RdKafka::ErrorCode& inErrorCode) const;
  void FlushMessageQueue();
  void InitialiseProducer();

  std::atomic_int32_t fPollTimeoutMs;
  std::unique_ptr<std::thread> fPollThread;
  std::queue<std::shared_ptr<GenericMessage>> fMessageBacklog;
  std::unique_ptr<KafkaProducerConfig> fKafkaConfig;
  std::unique_ptr<RdKafka::Producer> fProducer;
};

#endif
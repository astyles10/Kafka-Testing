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
  KafkaStream(std::shared_ptr<KafkaProducerConfig> inConfig);
  ~KafkaStream();

  void Notify(std::shared_ptr<GenericMessage> inMessage) override;

 private:
  void ProducerLoop();
  void ProcessMessageBacklog();
  void ProduceMessage();
  void HandleProduceErrorCode(const RdKafka::ErrorCode& inErrorCode) const;
  void HandleRetry(const RdKafka::ErrorCode& inErrorCode);
  bool DetermineIfRetryProduce(const RdKafka::ErrorCode& inErrorCode) const;
  void CeaseProducerThread();
  void FlushProducerQueue();
  void InitialiseProducer();
  void InitialiseProducerThread();

  std::atomic_int32_t fPollTimeoutMs;
  std::unique_ptr<std::thread> fProducerThread;
  std::atomic_bool fRunning;
  std::queue<std::shared_ptr<GenericMessage>> fMessageBacklog;
  std::shared_ptr<KafkaProducerConfig> fKafkaConfig;
  std::unique_ptr<RdKafka::Producer> fProducer;
};

#endif
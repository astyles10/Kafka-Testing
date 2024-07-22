#ifndef KAFKA_STREAM_HPP
#define KAFKA_STREAM_HPP

#include "IOQueue.hpp"
#include <rdkafkacpp.h>

class KafkaStream {
 public:
  KafkaStream();
  ~KafkaStream();
 private:
  void InitialiseStream();
  std::unique_ptr<RdKafka::Conf> fConfig;
  std::unique_ptr<RdKafka::Producer> fProducer;
  IOQueue fIoQueue;
};

#endif
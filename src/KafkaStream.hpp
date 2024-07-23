#ifndef KAFKA_STREAM_HPP
#define KAFKA_STREAM_HPP

#include "IOQueue.hpp"

#include <iostream>
#include <rdkafkacpp.h>

// TODO: Do something with this
class ExampleDeliveryReportCb : public RdKafka::DeliveryReportCb {
 public:
  void dr_cb(RdKafka::Message& inMessage) {
    if (inMessage.err()) {
      std::cerr << "!! Message delivery failed: " << inMessage.errstr() << "\n";
      return;
    }
    std::cout << "Message delivered to topic " << inMessage.topic_name() << " [" << inMessage.partition() << "] at offset " << inMessage.offset() << "\n";
  }
};

class KafkaStream {
 public:
  KafkaStream(const json& inConfig);
  ~KafkaStream();
  void Start();
  void Stop();
 private:
  void InitialiseConfig(const json& inConfig);
  void InitialiseStream();
  std::unique_ptr<RdKafka::Conf> fConfig;
  ExampleDeliveryReportCb fDeliveryCallback;
  std::unique_ptr<RdKafka::Producer> fProducer;
  IOQueue fIoQueue;
};

#endif
#ifndef KAFKA_STREAM_HPP
#define KAFKA_STREAM_HPP

#include "Observer.hpp"
#include "Streams/InputStream.hpp"

#include <iostream>
#include <nlohmann/json.hpp>
#include <rdkafkacpp.h>

using json = nlohmann::json;

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

class KafkaStream : public Observer {
 public:
  KafkaStream(const json& inConfig);
  ~KafkaStream();
  void Start();
  void Stop();

  void Notify(const GenericMessage& inMessage) override;

 private:
  void InitialiseConfig(const json& inConfig);
  void InitialiseStream();

  std::unique_ptr<RdKafka::Conf> fConfig;
  ExampleDeliveryReportCb fDeliveryCallback;
  std::unique_ptr<RdKafka::Producer> fProducer;
};

#endif
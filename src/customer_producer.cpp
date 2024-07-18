
#include <csignal>
#include <iostream>
#include <rdkafkacpp.h>

// Reference: https://github.com/confluentinc/librdkafka/blob/master/examples/producer.cpp

const static std::string gBroker = "localhost:9092";
const static std::string gTopic = "customer";

sig_atomic_t gRunning = true;

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

static void HandleSignal(int sig) {
  (void)sig;
  gRunning = false;
}

int main(int argc, char** argv) {
  RdKafka::Conf *aConfig = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
  std::string aError;

  if (aConfig->set("bootstrap.servers", gBroker, aError) != RdKafka::Conf::CONF_OK) {
    std::cerr << aError << "\n";
    exit(1);
  }

  signal(SIGTERM, HandleSignal);
  signal(SIGINT, HandleSignal);

  ExampleDeliveryReportCb aCallback;

  if (aConfig->set("dr_cb", &aCallback, aError) != RdKafka::Conf::CONF_OK) {
    std::cerr << aError << "\n";
    exit(1);
  }

  RdKafka::Producer* aProducer = RdKafka::Producer::create(aConfig, aError);
  if (!aProducer) {
    std::cerr << aError << "\n";
    exit(1);
  }

  delete aConfig;

  for (std::string line; gRunning && std::getline(std::cin, line);) {
    if (line.empty()) {
      aProducer->poll(0);
      continue;
    }
    RdKafka::ErrorCode aErrorCode = aProducer->produce(
      gTopic,
      // Use any partition - builtin partitioner will assign message to the topic based on message key, or random if key not set
      RdKafka::Topic::PARTITION_UA,
      // Copy the value
      RdKafka::Producer::RK_MSG_COPY,
      const_cast<char*>(line.c_str())
    );

  }

  delete aProducer;

  return 0;
}
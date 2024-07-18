
#include <csignal>
#include <cstdlib>
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

  // signal(SIGTERM, HandleSignal);
  // signal(SIGINT, HandleSignal);

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

  bool aRetry = false;

  for (std::string line; std::getline(std::cin, line);) {
    if (line.empty() && !aRetry) {
      aProducer->poll(0);
      continue;
    }
    aRetry = false;
    RdKafka::ErrorCode aErrorCode = aProducer->produce(
      gTopic,
      // Use any partition - builtin partitioner will assign message to the topic based on message key, or random if key not set
      RdKafka::Topic::PARTITION_UA,
      // Copy the value
      RdKafka::Producer::RK_MSG_COPY,
      const_cast<char*>(line.c_str()), line.size(),
      // Key
      NULL, 0,
      // Timestamp
      0,
      // Message headers
      NULL,
      // Per message opaque value passed to delivery report
      NULL
    );

    if (aErrorCode != RdKafka::ERR_NO_ERROR) {
      std::cerr << "!! Failed to produce to topic " << gTopic << ": " << RdKafka::err2str;

      /* If the internal queue is full, wait for
       * messages to be delivered and then retry.
       * The internal queue represents both
       * messages to be sent and messages that have
       * been sent or failed, awaiting their
       * delivery report callback to be called.
       *
       * The internal queue is limited by the
       * configuration property
       * queue.buffering.max.messages and queue.buffering.max.kbytes */
      if (aErrorCode == RdKafka::ERR__QUEUE_FULL) {
        aRetry = true;
        aProducer->poll(1000);
        continue;
      }
    }

    /* A producer application should continually serve
     * the delivery report queue by calling poll()
     * at frequent intervals.
     * Either put the poll call in your main loop, or in a
     * dedicated thread, or call it after every produce() call.
     * Just make sure that poll() is still called
     * during periods where you are not producing any messages
     * to make sure previously produced messages have their
     * delivery report callback served (and any other callbacks
     * you register). */
    aProducer->poll(0);
  }

  /* Wait for final messages to be delivered or fail.
   * flush() is an abstraction over poll() which
   * waits for all messages to be delivered. */
  std::cerr << "% Flushing final messages..." << std::endl;
  aProducer->flush(10 * 1000 /* wait for max 10 seconds */);

  if (aProducer->outq_len() > 0)
    std::cerr << "% " << aProducer->outq_len()
              << " message(s) were not delivered" << std::endl;

  delete aProducer;

  return 0;
}
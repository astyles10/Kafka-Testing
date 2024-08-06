#ifndef BASIC_DELIVERY_REPORT_CALLBACK
#define BASIC_DELIVERY_REPORT_CALLBACK

#include <iostream>
#include <rdkafkacpp.h>

// TODO: Do something with this
class BasicDeliveryReportCb : public RdKafka::DeliveryReportCb {
 public:
  void dr_cb(RdKafka::Message& inMessage) {
    if (inMessage.err()) {
      std::cerr << "!! Message delivery failed: " << inMessage.errstr() << "\n";
      return;
    }
    std::cout << "Message delivered to topic " << inMessage.topic_name() << " [" << inMessage.partition() << "] at offset " << inMessage.offset() << "\n";
  }
};

#endif

#ifndef OBSERVER_HPP
#define OBSERVER_HPP

#include "Messages/GenericMessage.hpp"

#include <memory>

class Observer {
 public:
  Observer() = default;
  ~Observer() = default;
  // TODO: Make this an async call
  virtual void Notify(std::shared_ptr<GenericMessage> inMessage) = 0;
};

#endif

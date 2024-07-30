#ifndef OBSERVER_HPP
#define OBSERVER_HPP

#include "Messages/GenericMessage.hpp"

class Observer {
 public:
  Observer() = default;
  ~Observer() = default;
  // TODO: Make this an async call
  virtual void Notify(const GenericMessage& inMessage) = 0;
};

#endif

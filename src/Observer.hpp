#ifndef OBSERVER_HPP
#define OBSERVER_HPP

#include "Messages/GenericMessage.hpp"

class Observer {
 public:
  Observer() = default;
  ~Observer() = default;
  virtual void Notify(GenericMessage& inMessage) = 0;
};

#endif

#ifndef INPUT_STREAM_HPP
#define INPUT_STREAM_HPP

#include "Observer.hpp"

#include <cctype>
#include <boost/asio.hpp>
#include <functional>
#include <istream>
#include <string>
#include <type_traits>

template<typename T>
  struct is_string : public std::disjunction<
    std::is_same<char *, std::decay_t<T>>,
    std::is_same<const char*, std::decay_t<T>>,
    std::is_same<std::string, std::decay_t<T>>
  > {};

template <class Message = GenericMessage>
class InputStream {
  static_assert(std::is_base_of<GenericMessage, Message>::value, "Type must derive from Generic");
 public:
  InputStream() = default;
  ~InputStream() = default;

  template <typename T>
  typename std::enable_if<is_string<T>::value, void>::type
  operator<<(const T& inString) {
    static_assert(is_string<decltype(inString)>::value, "Type must be a string or string literal");
    std::istringstream aStream(inString);
    const Message aMessage(aStream);
    NotifyObservers(aMessage);
  }

  template <typename T>
  typename std::enable_if<std::is_base_of<GenericMessage, T>::value, void>::type
  operator<<(const T& inMessage) {
    NotifyObservers(inMessage);
  }

  void Commit(const Message& inMessage) {
    NotifyObservers(inMessage);
  }

  void Commit(std::basic_ios<char>& inStream) {
    Message aMessage(inStream);
    NotifyObservers(aMessage);
  }

  void NotifyObservers(const Message& inMessage) {
    for (auto aObserver : fObservers) {
      if (auto aPtr = aObserver.lock()) {
        aPtr->Notify(inMessage);
      }
    }
  }

  void PushObserver(std::shared_ptr<Observer> inObserver) {
    fObservers.push_back(inObserver);
  }

 private:
  std::vector< std::weak_ptr<Observer> > fObservers;
};

#endif

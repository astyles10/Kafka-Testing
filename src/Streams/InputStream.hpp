#ifndef INPUT_STREAM_HPP
#define INPUT_STREAM_HPP

#include "Observer.hpp"

#include <cctype>
#include <boost/asio.hpp>
#include <functional>
#include <istream>
#include <string>
#include <type_traits>

template<typename U>
  struct is_string : public std::disjunction<
    std::is_same<char *, std::decay_t<U>>,
    std::is_same<const char*, std::decay_t<U>>,
    std::is_same<std::string, std::decay_t<U>>
  > {};

template <class T = GenericMessage>
class InputStream {
  static_assert(std::is_base_of<GenericMessage, T>::value, "Type must derive from Generic");
 public:
  InputStream() = default;
  // ~InputStream() = default;
  ~InputStream() {
    std::cout << "InputStream destructor called\n";
  }

  // TODO: this works but only checks at compile time.
  template <typename U>
  void operator<<(U& inString) {
    static_assert(is_string<decltype(inString)>::value, "Type must be a string or string literal");
    std::cout << " Operator <<: " << inString << std::endl;
  }

  // template <class U, typename std::enable_if_t<std::is_arithmetic<U>::value>* = nullptr>
  // void operator<<(U& inValue) {
  //   std::cout << " Operator <<: " << inValue << std::endl;
  // }

  void Commit(T& inMessage) {
    NotifyObservers(inMessage);
  }

  void Commit(std::basic_ios<char>& inStream) {
    T aMessage(inStream);
    NotifyObservers(aMessage);
  }

  void NotifyObservers(T& inMessage) {
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

#include "Observer.hpp"
#include "Messages/JsonMessage.hpp"
#include "Streams/InputStream.hpp"

#include <fstream>
#include <iostream>
#include <memory>
#include <type_traits>

template <class T = GenericMessage>
class DataConsumer : public Observer {
 public:
  DataConsumer(const std::string& inName, InputStream<T>& inStream) : fInputStream(inStream) {
  }
  ~DataConsumer() {
  }

  void Notify(const GenericMessage& inMessage) {
    std::cout << fName << " got message: " << inMessage.Get() << std::endl;
  }

 private:
  const std::string fName;
  InputStream<T>& fInputStream;
};

class TestInputs {
 public:
  TestInputs() = default;
  ~TestInputs() = default;

  // The = 0 is a default template parameter based on the class of the 2nd parameter
  template <typename X, typename std::enable_if<std::numeric_limits<X>::is_integer, int>::type = 0>
  // typename std::enable_if<std::numeric_limits<X>::is_integer, int>::type
  X operator<<(const X& inValue) {
    std::cout << "operator << got value " << inValue << " with type: " << typeid(inValue).name() << std::endl;
    return inValue;
  }

 private:
};

int main(int argc, char **argv) {
  // The << operator requires an integer type as input:
  uint16_t aVal = 65000;

  // The operator function returns the type of the second argument to std::enable_if_t (int in this case):
  // --> typename std::enable_if<std::numeric_limits<X>::is_integer, int>::type
  // So if we don't care about a return type, it should be set to void in the enable_if template declaration.
  TestInputs aTestOperator;
  auto aShouldBeInt = (aTestOperator << aVal);
  std::cout << "aShouldBeInt type = " << typeid(aShouldBeInt).name() << std::endl;

  std::enable_if<true, double>::type aCastToDouble = (aTestOperator << aVal);
  std::cout << "aCastToDouble Type = " << typeid(aCastToDouble).name() << std::endl;

  InputStream aGenericStream;
  auto aConsumer = std::make_shared<DataConsumer<GenericMessage>>("Generic Stream", aGenericStream);
  aGenericStream.PushObserver(aConsumer);

  // The problem originally was that the << overload definition was trying to return two types:
  // void and enable_if::type
  std::string aString = "std::string input\n";
  aGenericStream << aString;
  aGenericStream << "string literal input!\n";

  GenericMessage aMessage;
  const std::string aStringIntoMessage = "generic message string\n";
  aMessage << aStringIntoMessage;
  aGenericStream << aMessage;

  /*
    Potential issue with shared pointers and circular referencing
    DataConsumer has a reference to InputStream
    InputStream owns a shared_ptr to DataConsumer

    DataConsumer should probably have a shared/weak pointer to InputStream
    Need to ensure memory/thread safety for the Observer Notify() function
    E.g. What happens if DataConsumer is destroyed when Notify is called?
    A weak pointer implementation + check if exists might be enough
   */

  InputStream<JsonMessage> aJsonStream;
  auto aJsonConsumer = std::make_shared<DataConsumer<JsonMessage>>("Json Stream", aJsonStream);
  std::ifstream aFile("test.json");
  aJsonStream.PushObserver(aJsonConsumer);
  aJsonStream.Commit(aFile);
  aJsonStream << "{\"testJson\": true}";

  return 0;
}
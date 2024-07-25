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
    std::cout << "Consumer destructor called\n";
  }

  void Notify(GenericMessage& inMessage) {
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

  // template<class TestInputs, class T, typename std::enable_if<std::is_integral<T>::value, void>::type>
  // template <typename T = typename std::enable_if<std::is_integral<int>::value>::type>
  // typename std::enable_if<std::is_integral<T>::value, void>::type
  // void operator<<(T& inValue) {
  //   std::cout << __FUNCTION__ << ": Operator got " << inValue << std::endl;
  // }
 private:
};

int main(int argc, char **argv) {
  InputStream aGenericStream;
  auto aConsumer = std::make_shared<DataConsumer<GenericMessage>>("Generic Stream", aGenericStream);
  aGenericStream.PushObserver(aConsumer);
  GenericMessage aMessage;
  std::string aString = "My <<'d message\n";
  aMessage << aString;
  std::cout << aMessage.Get();
  aGenericStream << "into the generic stream we go\n";

  TestInputs aTestInput;
  // aTestInput << int(100);
  // aGenericStream << aString;

  // aGenericStream.Commit(std::cin);

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

  return 0;
}
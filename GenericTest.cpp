#include <boost/asio.hpp>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

class Generic {
 public:
  Generic() = default;
  ~Generic() = default;
  virtual std::string Get() = 0;
};

class Specific : public Generic {
 public:
  Specific(int inValue) {
    fData = inValue;
  }
  std::string Get() {
    return std::to_string(fData);
  }
 private:
  int fData;
};

class JsonMessage : public Generic {
 public:
  JsonMessage(const nlohmann::json& inJson) {
    fData = inJson;
  }
  std::string Get() {
    return fData.dump();
  }
 private:
  nlohmann::json fData;
};

void PrintGeneric(Generic& inGeneric) {
  std::cout << inGeneric.Get() << std::endl;
}

void TakeInIstream(std::basic_ios<char>& inStream) {
  // nlohmann::json aJson;
  // aJson.parse(inStream);
  std::stringstream aStr;
  aStr << inStream.rdbuf();
  std::cout << "My string: " << aStr.str() << std::endl;
}

int main(int argc, char** argv) {
  Specific aGeneric(10);
  PrintGeneric(aGeneric);

  JsonMessage aJsonStr({{"one", 1}, {"two", 2}});
  PrintGeneric(aJsonStr);

  std::string aString;

  std::ifstream aFile("txt_file.txt");
  TakeInIstream(aFile);

  // std::string aLine;
  // std::getline(std::cin, aLine);
  // std::cout << aLine << std::endl;

  TakeInIstream(std::cin);

  std::stringstream aStrStr;
  aStrStr << "my stringstream\n";
  TakeInIstream(aStrStr);

  using namespace boost::asio::ip;
  boost::asio::io_context aIoContext;
  tcp::endpoint aEndpoint(tcp::v4(), 4444);
  tcp::acceptor aAcceptor(aIoContext, aEndpoint);
  for (;;) {
    tcp::iostream aStream;
    boost::system::error_code aErrorCode;
    aAcceptor.accept(aStream.socket(), aErrorCode);
    if (!aErrorCode) {
      TakeInIstream(aStream);
    }
  }

  return 0;
}
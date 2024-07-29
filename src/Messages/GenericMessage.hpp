#ifndef STREAM_DATA_HPP
#define STREAM_DATA_HPP

#include <iostream>
#include <sstream>

/* 
  GenericMessage: Generic data structure.
  Provides interface for accessing any type data format such as JSON, XML, etc.
 */

class GenericMessage {
 public:
  GenericMessage() = default;
  GenericMessage(std::basic_ios<char>& inStream) {
    Consume(inStream);
  }
  virtual ~GenericMessage() = default;

  void operator<<(std::istream& aStr) {
    Consume(aStr);
  }

  void operator<<(const std::string& aStr) {
    std::stringstream aStream;
    aStream << aStr;
    Consume(aStream);
  }

  // TODO: Instead of copying data, maybe the data could be moved or passed directly to an output stream?
  virtual GenericMessage Create(std::basic_ios<char>& inStream) {
    Consume(inStream);
    return *this;
  };

  virtual const std::string Get() const { return fData; }

 protected:
  virtual void Consume(std::basic_ios<char>& inStream) {
    std::stringstream aStrStr;
    aStrStr << inStream.rdbuf();
    fData = aStrStr.str();
  }
 private:
  std::string fData;
};

#endif
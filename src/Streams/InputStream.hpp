#ifndef INPUT_STREAM_HPP
#define INPUT_STREAM_HPP

#include <istream>
#include <string>
#include <boost/asio.hpp>

// TODO: Need to integrate GenericMessage type casting into Streams
class InputStream {
 public:
  InputStream(std::basic_ios<char>& inIoStream);
  virtual ~InputStream();
 protected:
  std::basic_ios<char>& fIoStream;
 private:
  virtual void Consume();
};

#endif

#include "InputStream.hpp"

InputStream::InputStream(std::basic_ios<char>& inIoStream) : fIoStream(inIoStream) {
}

InputStream::~InputStream() {

}

void InputStream::Consume() {
  // Depends on input type, this should be pure virtual
}

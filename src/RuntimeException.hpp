#ifndef RUNTIME_EXCEPTION_HPP
#define RUNTIME_EXCEPTION_HPP

#include <exception>

// TODO: Log exceptions to an output stream - stdout, kafka, central logging, etc.
class RuntimeException : public std::exception {

};

#endif

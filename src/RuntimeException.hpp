#ifndef RUNTIME_EXCEPTION_HPP
#define RUNTIME_EXCEPTION_HPP

#include <exception>

// TODO: Log exceptions to an output stream - stdout, kafka, central logging, etc.
// See https://stackoverflow.com/questions/52357/what-is-the-point-of-clog
// This probably shouldn't be an exception handler though - what if we get an exception in the logger?
class RuntimeException : public std::exception {

};

#endif

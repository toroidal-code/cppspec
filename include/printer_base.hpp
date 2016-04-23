#ifndef CPP_SPEC_PRINTER_BASE_HPP
#define CPP_SPEC_PRINTER_BASE_HPP
#include "runnable.hpp"

class Description;
class ItBase;

struct BasePrinter {
  enum class Mode { terse, verbose, TAP };
  Mode mode;
  BasePrinter(Mode mode) : mode(mode) {};
  virtual void print(Description &runnable) = 0;
  virtual void print(ItBase &runnable) = 0;
  virtual void print(std::string message) = 0;
  virtual void print_failure(std::string string) = 0;
  virtual void flush() = 0;
};

#endif //CPP_SPEC_PRINTER_BASE_HPP

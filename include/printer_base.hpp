#ifndef CPPSPEC_PRINTER_BASE_HPP
#define CPPSPEC_PRINTER_BASE_HPP

namespace CppSpec {

class Description;
class BaseIt;

struct BasePrinter {
  enum class Mode { terse, verbose, TAP };
  Mode mode;
  BasePrinter(Mode mode) : mode(mode){};
  virtual void print(Description &runnable) = 0;
  virtual void print(BaseIt &runnable) = 0;
  virtual void print(std::string message) = 0;
  virtual void print_failure(std::string string) = 0;
  virtual void flush() = 0;
};

} // ::CppSpec
#endif  // CPPSPEC_PRINTER_BASE_HPP

/** @file */
#ifndef CPPSPEC_FORMATTERS_FORMATTERS_BASE
#define CPPSPEC_FORMATTERS_FORMATTERS_BASE
#include <iostream>
#include "term_colors.hpp"

namespace CppSpec {
class Description;
class BaseIt;

namespace Formatters {

class BaseFormatter {
 protected:
  std::ostream &out_stream;
  int test_counter = 1;
  bool multiple = false;
  bool color_output;

 public:
  BaseFormatter(std::ostream &out_stream = std::cout, bool color = true)
      : out_stream(out_stream), color_output(color){};
  BaseFormatter(const BaseFormatter &) = default;
  BaseFormatter(const BaseFormatter &copy, std::ostream &out_stream)
      : out_stream(out_stream),
        test_counter(copy.test_counter),
        multiple(copy.multiple),
        color_output(copy.color_output){};

  virtual ~BaseFormatter() = default;

  virtual void format(Description &description){};
  virtual void format(BaseIt &it){};
  virtual void format(std::string message) {
    out_stream << message << std::endl;
  };
  virtual void format_failure(std::string message){};
  virtual void flush(){};
  virtual void cleanup(){};

  BaseFormatter &set_multiple(bool value);
  BaseFormatter &set_color_output(bool value);

  int get_and_increment_test_counter() { return test_counter++; }
  void reset_test_counter() { test_counter = 1; }
};

BaseFormatter &BaseFormatter::set_multiple(bool multiple) {
  this->multiple = multiple;
  return *this;
}
BaseFormatter &BaseFormatter::set_color_output(bool value) {
  this->color_output = value;
  return *this;
}

}  // ::Formatters
}  // ::CppSpec
#endif  // CPPSPEC_FORMATTERS_FORMATTERS_BASE

/** @file */
#pragma once

#include <cstdio>
#include <format>
#include <iostream>
#include <string>

#include "description.hpp"
#include "it_base.hpp"
#include "runnable.hpp"
#include "term_colors.hpp"

extern "C" {
#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif
}

namespace CppSpec {
class Description;
class ItBase;

inline bool is_terminal() {
#ifdef _WIN32
  return _isatty(_fileno(stdout)) != 0;
#else
  return isatty(fileno(stdout)) != 0;
#endif
}
namespace Formatters {

class BaseFormatter {
 protected:
  std::ostream& out_stream;
  int test_counter = 1;
  bool color_output;

 public:
  explicit BaseFormatter(std::ostream& out_stream = std::cout, bool color = is_terminal())
      : out_stream(out_stream), color_output(color) {}
  BaseFormatter(const BaseFormatter&) = default;
  BaseFormatter(const BaseFormatter& copy, std::ostream& out_stream)
      : out_stream(out_stream), test_counter(copy.test_counter), color_output(copy.color_output) {}

  virtual ~BaseFormatter() = default;

  void format(const Runnable& runnable) {
    if (const auto* description = dynamic_cast<const Description*>(&runnable)) {
      format(*description);
    } else if (const auto* it = dynamic_cast<const ItBase*>(&runnable)) {
      format(*it);
    }
    format_children(runnable);
  }

  void format_children(const Runnable& runnable) {
    for (const auto& child : runnable.get_children()) {
      if (const auto* runnable = dynamic_cast<const Runnable*>(child.get())) {
        this->format(*runnable);
      }
    }
  }

  virtual void format(const Description& /* description */) {}
  virtual void format(const ItBase& /* it */) {}
  virtual void cleanup() {}

  BaseFormatter& set_color_output(bool value);

  int get_and_increment_test_counter() { return test_counter++; }
  void reset_test_counter() { test_counter = 1; }

  const char* set_color(const char* color) {
    if (!color_output) {
      return "";  // No color output
    }
    return color;
  }

  const char* status_color(Result::Status status) {
    if (!color_output) {
      return "";  // No color output
    }
    switch (status) {
      case Result::Status::Success:
        return GREEN;
      case Result::Status::Failure:
        return RED;
      case Result::Status::Error:
        return MAGENTA;
      case Result::Status::Skipped:
        return YELLOW;
    }
    return "";  // Default to no color
  }

  const char* reset_color() { return color_output ? RESET : ""; }

  /**
   * @brief Prefix a result's message with the file and line of the expectation that reported it.
   *
   * Every formatter that prints failure messages as text uses this, so an editor or terminal can jump from any of
   * them to the failing expectation.
   *
   * @param result the result to describe
   *
   * @return `file:line: message`
   */
  [[nodiscard]] static std::string located_message(const Result& result) {
    return std::format("{}:{}: {}", result.get_location().file_name(), result.get_location().line(),
                       result.get_message());
  }
};

inline BaseFormatter& BaseFormatter::set_color_output(bool value) {
  this->color_output = value;
  return *this;
}

}  // namespace Formatters
}  // namespace CppSpec

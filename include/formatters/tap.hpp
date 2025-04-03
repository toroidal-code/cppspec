/** @file */
#pragma once

#include <sstream>
#include <string>

#include "formatters_base.hpp"
#include "term_colors.hpp"

namespace CppSpec::Formatters {
struct TAP final : public BaseFormatter {
  bool first = true;
  std::ostringstream buffer;

  std::string result_to_yaml(const Result& result);
  void format(Description& description) override;
  void format(ItBase& it) override;
};

inline std::string TAP::result_to_yaml(const Result& result) {
  if (result.is_success()) {
    return std::string();
  }

  std::ostringstream oss;
  oss << "  " << "---" << std::endl;
  oss << "  " << "message: " << "'" << result.get_message() << "'" << std::endl;
  oss << "  " << "severity: failure" << std::endl;
  oss << "  " << "at:" << std::endl;
  oss << "  " << "  " << "file: " << result.get_location().file_name() << std::endl;
  oss << "  " << "  " << "line: " << result.get_location().line() << std::endl;
  // oss << "  " << "data:" << std::endl;
  // oss << "  " << "  " << "expected: " << result.get_expected() << std::endl;
  // oss << "  " << "  " << "got: " << result.get_actual() << std::endl;
  oss << "  " << "..." << std::endl;
  return oss.str();
}

inline void TAP::format(Description& description) {
  if (!first && !description.has_parent()) {
    std::string str = buffer.str();
    std::ostringstream oss;
    if (str[0] == '\n') {
      oss << str[0];
    }

    if (color_output) {
      oss << GREEN;
    }
    oss << "1.." << test_counter - 1;
    if (color_output) {
      oss << RESET;
    }
    oss << std::endl;

    oss << ((str[0] == '\n') ? str.substr(1) : str);

    std::cout << oss.str() << std::flush;
    first = false;
    test_counter = 1;
    buffer = std::ostringstream();
  }

  if (first) {
    first = false;
  }
}

inline void TAP::format(ItBase& it) {
  std::string description{it.get_description()};

  // Build up the description for the test by ascending the
  // execution tree and chaining the individual descriptions together

  for (auto parent = it.get_parent_as<Description>(); parent != nullptr;
       parent = parent->get_parent_as<Description>()) {
    description = std::string(parent->get_description()) + " " + description;
  }

  if (color_output) {
    buffer << (it.get_result().status() ? GREEN : RED);
  }
  buffer << (it.get_result().status() ? "ok" : "not ok");
  if (color_output) {
    buffer << RESET;
  }
  buffer << " " << get_and_increment_test_counter() << " - " << description << std::endl;
  buffer << result_to_yaml(it.get_result());
}

static TAP tap;

}  // namespace CppSpec::Formatters

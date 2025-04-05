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

  ~TAP() { flush(); }

  std::string result_to_yaml(const Result& result);
  void format(const Description& description) override;
  void format(const ItBase& it) override;
  void flush();
};

inline std::string TAP::result_to_yaml(const Result& result) {
  if (result.is_success()) {
    return {};
  }

  auto message = result.get_message();

  std::ostringstream oss;
  oss << "  " << "---" << std::endl;
  if (message.contains("\n")) {
    oss << "  " << "message: |" << std::endl;
    std::string indented_message = message;  // split on newlines and indent
    std::string::size_type pos = 0;
    while ((pos = indented_message.find('\n', pos)) != std::string::npos) {
      indented_message.replace(pos, 1, "\n    ");
      pos += 2;  // Skip over the newline and the space we just added
    }
    oss << "  " << "  " << indented_message << std::endl;
  } else {
    oss << "  " << "message: " << '"' << result.get_message() << '"' << std::endl;
  }

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

inline void TAP::flush() {
  std::string str = buffer.str();
  std::ostringstream oss;
  if (str[0] == '\n') {
    oss << str[0];
  }

  if (test_counter != 1) {
    if (color_output) {
      oss << GREEN;
    }
    oss << "1.." << test_counter - 1;
    if (color_output) {
      oss << RESET;
    }
    oss << std::endl;
  }

  oss << ((str[0] == '\n') ? str.substr(1) : str);

  std::cout << oss.str() << std::flush;
  first = false;
  reset_test_counter();
  buffer = std::ostringstream();
}

inline void TAP::format(const Description& description) {
  if (!first && !description.has_parent()) {
    flush();
  }

  if (first) {
    first = false;
  }
}

inline void TAP::format(const ItBase& it) {
  // Build up the description for the test by ascending the
  // execution tree and chaining the individual descriptions together
  std::forward_list<std::string> descriptions;

  descriptions.push_front(it.get_description());
  for (const auto* parent = it.get_parent_as<Description>(); parent->has_parent();
       parent = parent->get_parent_as<Description>()) {
    descriptions.push_front(parent->get_description());
  }

  std::string description = Util::join(descriptions, " ");

  buffer << status_color(it.get_result().status());
  buffer << (it.get_result().is_success() ? "ok" : "not ok");
  buffer << reset_color();
  buffer << " " << get_and_increment_test_counter() << " - " << description << std::endl;
  buffer << result_to_yaml(it.get_result());
}

static TAP tap;

}  // namespace CppSpec::Formatters

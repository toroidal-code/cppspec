/** @file */
#ifndef CPPSPEC_FORMATTERS_TAP_HPP
#define CPPSPEC_FORMATTERS_TAP_HPP
#pragma once

#include <list>
#include <string>

#include "class_description.hpp"
#include "formatters_base.hpp"


namespace CppSpec {
namespace Formatters {

// The TAP format makes things a little tricky
class TAP : public BaseFormatter {
  bool first = true;
  std::ostringstream buffer;
  std::list<std::string> failure_messages;

 public:
  void format(const Description &description) override;
  void format(const ItBase &it) override;
  void format(std::string message) override;
  void format_failure(std::string message) override;
  void flush() override;
};

inline void TAP::format(const Description &description) {
  if (!first && description.get_parent() == nullptr) out_stream << std::endl;
  if (first) this->first = false;
}

inline void TAP::format(const ItBase &it) {
  std::string description = it.get_description();

  // Build up the description for the test by ascending the
  // execution tree and chaining the individual descriptions together

  for (const Description *parent = it.get_parent_as<const Description *>();
       parent != nullptr;
       parent = parent->get_parent_as<const Description *>()) {
    description = parent->get_description() + " " + description;
  }

  if (color_output) buffer << (it.get_status() ? GREEN : RED);
  buffer << (it.get_status() ? "ok" : "not ok");
  if (color_output) buffer << RESET;
  buffer << " " << get_and_increment_test_counter() << " - " << description
         << std::endl;
  if (not failure_messages.empty()) {
    for (auto failure : failure_messages) buffer << failure;
    failure_messages.clear();
  }
}

inline void TAP::format(std::string message) { buffer << message << std::endl; }

inline void TAP::format_failure(std::string message) {
  std::ostringstream oss;
  std::istringstream iss(message);
  std::string line;
  while (std::getline(iss, line)) {
    oss << "    " << line << std::endl;
  }
  failure_messages.push_back(oss.str());
}

inline void TAP::flush() {
  std::string str = buffer.str();
  std::ostringstream oss;
  if (str[0] == '\n') {
    oss << str.substr(0, 1) << GREEN << "1.." << test_counter - 1 << RESET
        << std::endl
        << str.substr(1);
  } else {
    oss << GREEN << "1.." << test_counter - 1 << RESET << std::endl << str;
  }
  std::cout << oss.str() << std::flush;
  first = false;
  test_counter = 1;
  buffer = std::ostringstream();
}

static TAP tap;

}  // namespace Formatters
}  // namespace CppSpec
#endif  // CPPSPEC_FORMATTERS_TAP_HPP

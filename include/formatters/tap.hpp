/** @file */
#pragma once

#include <list>
#include <string>

#include "class_description.hpp"
#include "formatters_base.hpp"
#include "term_colors.hpp"

namespace CppSpec::Formatters {

// The TAP format makes things a little tricky
class TAP : public BaseFormatter {
  bool first = true;
  std::ostringstream buffer;
  std::list<std::string> failure_messages{};

 public:
  void format(const Description &description) override;
  void format(const ItBase &it) override;
  void format(const std::string &message) override;
  void format_failure(const std::string &message) override;
  void flush() override;
};

inline void TAP::format(const Description &description) {
  if (!first && description.get_parent() == nullptr) {
    out_stream << std::endl;
  }
  if (first) {
    this->first = false;
  }
}

inline void TAP::format(const ItBase &it) {
  std::string description{it.get_description()};

  // Build up the description for the test by ascending the
  // execution tree and chaining the individual descriptions together

  for (const auto *parent = it.get_parent_as<const Description *>(); parent != nullptr;
       parent = parent->get_parent_as<const Description *>()) {
    description = std::string(parent->get_description()) + " " + description;
  }

  if (color_output) {
    buffer << (it.get_status() ? GREEN : RED);
  }
  buffer << (it.get_status() ? "ok" : "not ok");
  if (color_output) {
    buffer << RESET;
  }
  buffer << " " << get_and_increment_test_counter() << " - " << description << std::endl;
  if (not failure_messages.empty()) {
    for (const auto &failure : failure_messages) {
      buffer << failure;
    }
    failure_messages.clear();
  }
}

inline void TAP::format(const std::string &message) { buffer << message << std::endl; }

inline void TAP::format_failure(const std::string &message) {
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

static TAP tap;

}  // namespace CppSpec::Formatters

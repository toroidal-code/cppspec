/** @file */
#ifndef CPPSPEC_FORMATTERS_TAP_HPP
#define CPPSPEC_FORMATTERS_TAP_HPP
#include <list>
#include "formatters_base.hpp"
#include "class_description.hpp"

namespace CppSpec {
namespace Formatters {

// The TAP format makes things a little tricky
class TAP : public BaseFormatter {
  bool first = true;
  std::ostringstream buffer;
  std::list<std::string> failure_messages;

 public:
  void format(Description &description) override;
  void format(BaseIt &it) override;
  void format(std::string message) override;
  void format_failure(std::string message) override;
  void flush() override;
};
inline void TAP::format(Description &description) {
  if (!first && description.get_parent() == nullptr) out_stream << std::endl;
  if (first) this->first = false;
}
inline void TAP::format(BaseIt &it) {
  std::string description = it.get_descr();

  // Build up the description for the test by ascending the
  // execution tree and chaining the individual descriptions together
  Description *parent = static_cast<Description *>(it.get_parent());
  while (parent != nullptr) {
    description = parent->get_descr() + " " + description;
    parent = static_cast<Description *>(parent->get_parent());
  }

  if (color_output) buffer << (it.get_status() ? GREEN: RED);
  buffer << (it.get_status() ? "ok" : "not ok");
  if (color_output) buffer << RESET;
  buffer << " " << get_and_increment_test_counter() << " - " << description << std::endl;
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

} // ::Formatters
} // ::CppSpec
#endif  // CPPSPEC_FORMATTERS_TAP_HPP

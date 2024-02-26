/** @file */
#pragma once

#include <list>
#include <string>

#include "argparse.hpp"
#include "class_description.hpp"
#include "formatters_base.hpp"
#include "it_base.hpp"

namespace CppSpec::Formatters {

class Verbose : public BaseFormatter {
  bool first = true;
  std::list<std::string> failure_messages{};

 public:
  Verbose() = default;
  explicit Verbose(std::ostream &out_stream) : BaseFormatter(out_stream) {}
  Verbose(const BaseFormatter &base, std::ostream &out_stream) : BaseFormatter(base, out_stream) {}
  explicit Verbose(const BaseFormatter &base) : BaseFormatter(base) {}

  void format(const Description &description) override;
  void format(const ItBase &description) override;
  void format_failure(const std::string &message) override;
  void format_failure_messages();
};

inline void Verbose::format(const Description &description) {
  if (!first && !description.has_parent()) {
    out_stream << std::endl;
  }
  out_stream << description.padding() << description.get_description() << description.get_subject_type() << std::endl;
  if (first) {
    this->first = false;
  }
}

inline void Verbose::format(const ItBase &it) {
  if (color_output) {
    out_stream << (it.get_status() ? GREEN : RED);
  }
  out_stream << it.padding() << it.get_description() << std::endl;
  if (color_output) {
    out_stream << RESET;
  }

  // Print any failures if we've got them
  // 'it' having a bad status necessarily
  // implies that there are failure messages
  format_failure_messages();
  get_and_increment_test_counter();
}

inline void Verbose::format_failure(const std::string &message) { failure_messages.push_back(message); }

// TODO: Compare this with Progress::format_failure_messages
inline void Verbose::format_failure_messages() {
  if (not failure_messages.empty()) {  // If we have any failures to format
    if (color_output) {
      out_stream << RED;  // make them red
    }
    out_stream << Util::join(failure_messages,
                             "\n")  // separated by a blank line
               << std::endl;        // newline
    if (color_output) {
      out_stream << RESET;
    }
    failure_messages.clear();  // Finally, clear the failures list.
  }
}

static Verbose verbose;

}  // namespace CppSpec::Formatters


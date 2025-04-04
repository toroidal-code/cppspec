/** @file */
#pragma once

#include "formatters_base.hpp"
#include "it_base.hpp"
#include "term_colors.hpp"

namespace CppSpec::Formatters {

class Verbose : public BaseFormatter {
  bool first = true;

 public:
  Verbose() = default;
  explicit Verbose(std::ostream& out_stream) : BaseFormatter(out_stream) {}
  Verbose(const BaseFormatter& base, std::ostream& out_stream) : BaseFormatter(base, out_stream) {}
  explicit Verbose(const BaseFormatter& base) : BaseFormatter(base) {}

  void format(Description& description) override;
  void format(ItBase& it) override;
};

inline void Verbose::format(Description& description) {
  if (!first && !description.has_parent()) {
    out_stream << std::endl;
  }
  out_stream << description.padding() << description.get_description() << description.get_subject_type() << std::endl;
  if (first) {
    this->first = false;
  }
}

inline void Verbose::format(ItBase& it) {
  if (color_output) {
    out_stream << (it.get_result().status() ? GREEN : RED);
  }
  out_stream << it.padding() << it.get_description() << std::endl;
  if (color_output) {
    out_stream << RESET;
  }

  // Print any failures if we've got them
  // 'it' having a bad status necessarily
  // implies that there are failure messages
  for (const auto& result : it.get_results()) {
    if (result.is_failure()) {
      if (color_output) {
        out_stream << RED;  // make them red
      }
      out_stream << result.get_message() << std::endl;
      if (color_output) {
        out_stream << RESET;
      }
    }
  }

  get_and_increment_test_counter();
}

static Verbose verbose;

}  // namespace CppSpec::Formatters

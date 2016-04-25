/** @file */
#ifndef CPPSPEC_FORMATTERS_VERBOSE_HPP
#define CPPSPEC_FORMATTERS_VERBOSE_HPP
#include <list>
#include "formatters_base.hpp"
#include "class_description.hpp"

namespace CppSpec {
namespace Formatters {

class Verbose : public BaseFormatter {
  bool first = true;
  std::list<std::string> failure_messages;

 public:
  Verbose() = default;
  Verbose(std::ostream &out_stream) : BaseFormatter(out_stream){};
  Verbose(const BaseFormatter &base, std::ostream &out_stream)
      : BaseFormatter(base, out_stream){};
  Verbose(const BaseFormatter &base) : BaseFormatter(base){};

  void format(Description &description) override;
  void format(BaseIt &description) override;
  void format_failure(std::string message) override;
  void format_failure_messages();
};

inline void Verbose::format(Description &description) {
  if (!first && !description.has_parent()) out_stream << std::endl;
  out_stream << description.padding() << description.get_descr()
             << description.get_subject_type() << std::endl;
  if (first) this->first = false;
}

inline void Verbose::format(BaseIt &it) {
  if (color_output) out_stream << (it.get_status() ? GREEN : RED);
  out_stream << it.padding() << it.get_descr() << std::endl;
  if (color_output) out_stream << RESET;

  // Print any failures if we've got them
  // 'it' having a bad status necessarily
  // implies that there are failure messages
  format_failure_messages();
  get_and_increment_test_counter();
}

inline void Verbose::format_failure(std::string message) {
  failure_messages.push_back(message);
}

// TODO: Compare this with Progress::format_failure_messages
inline void Verbose::format_failure_messages() {
  if (not failure_messages.empty()) {     // If we have any failures to format
    if (color_output) out_stream << RED;  // make them red
    out_stream << Util::join(failure_messages,
                             "\n")  // separated by a blank line
               << std::endl;        // newline
    if (color_output) out_stream << RESET;
    failure_messages.clear();  // Finally, clear the failures list.
  }
}

static Verbose verbose;

}  // ::Formatters
}  // ::CppSpec
#endif  // CPPSPEC_FORMATTERS_VERBOSE_HPP

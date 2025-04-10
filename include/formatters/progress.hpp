/** @file */
#pragma once

#include <forward_list>
#include <list>
#include <string>

#include "term_colors.hpp"
#include "verbose.hpp"

namespace CppSpec::Formatters {

// The TAP format makes things a little tricky
class Progress : public BaseFormatter {
  std::list<std::string> baked_failure_messages;

  std::string prep_failure_helper(const ItBase& it);

 public:
  ~Progress() override {
    format_failure_messages();  // Print any failures that we have
  }
  void format(const ItBase& it) override;

  void format_failure_messages();
  void prep_failure(const ItBase& it);

  static char status_char(Result::Status status) {
    switch (status) {
      case Result::Status::Success:
        return '.';
      case Result::Status::Failure:
        return 'F';
      case Result::Status::Error:
        return 'E';
      case Result::Status::Skipped:
        return 'S';
    }
    return '.';  // Default to success if status is unknown
  }
};

/** @brief An assistant function for prep_failure to reduce complexity */
inline std::string Progress::prep_failure_helper(const ItBase& it) {
  // a singly-linked list to act as a LIFO queue
  std::forward_list<std::string> list;

  // a temporary stream for our helper formatter to write to
  std::ostringstream temp_stream;

  // A lambda to clean up some code reuse
  auto push_and_clear = [&]() {
    list.push_front(temp_stream.str());  // push the stream's contents onto the
    temp_stream.clear();                 // clear the stream's flags
    temp_stream.str("");                 // reset it to empty
  };

  // Format the 'it' example first
  temp_stream << it.padding() << it.get_description() << std::endl;
  push_and_clear();

  // Verbose already has the majority of what we need
  // defined, so we'll use it.
  Verbose helper_formatter = Verbose(*this, temp_stream);

  // Ascend the tree to the root, formatting the nodes and
  // enqueing each formatted string as we go.
  const auto* parent = it.get_parent_as<Description>();

  do {
    helper_formatter.format(*parent);  // Format the node
    push_and_clear();
  } while ((parent = dynamic_cast<const Description*>(parent->get_parent())) != nullptr);

  return Util::join(list);  // squash the list of strings and return it.
}

inline void Progress::prep_failure(const ItBase& it) {
  std::list<std::string> raw_failure_messages;  // raw failure messages
  std::ranges::transform(it.get_results(), std::back_inserter(raw_failure_messages),
                         [](const Result& result) { return result.get_message(); });

  std::ostringstream string_builder;                               // oss is used as the local string builder
  string_builder << set_color(RED);                                // if we're doing color, make it red
  string_builder << "Test number " << test_counter << " failed:";  // Tell us what test # failed
  string_builder << reset_color();                                 // reset the color
  string_builder << prep_failure_helper(it);
  string_builder << set_color(RED);
  string_builder << Util::join_endl(raw_failure_messages);
  string_builder << reset_color();  // reset the color
  string_builder << std::endl;

  raw_failure_messages.clear();
  baked_failure_messages.push_back(string_builder.str());
}

inline void Progress::format(const ItBase& it) {
  out_stream << status_color(it.get_result().status());
  out_stream << status_char(it.get_result().status());
  out_stream << reset_color();
  out_stream << std::flush;

  if (it.get_result().status() == Result::Status::Failure) {
    prep_failure(it);
  }
  get_and_increment_test_counter();
}

inline void Progress::format_failure_messages() {
  if (baked_failure_messages.empty()) {
    out_stream << std::endl;  // If we don't have any failures, just print a blank line.
    return;
  }

  // If we have any failures to format
  for (const std::string& message : baked_failure_messages) {
    out_stream << std::endl;
    out_stream << message;  // separated by a blank line
  }
  baked_failure_messages.clear();  // Finally, clear the failures list.
}

static Progress progress;

}  // namespace CppSpec::Formatters

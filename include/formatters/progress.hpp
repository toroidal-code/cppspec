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
  std::list<std::string> baked_failure_messages{};
  std::list<std::string> raw_failure_messages{};

  std::string prep_failure_helper(ItBase& it);

 public:
  ~Progress() override {
    format_failure_messages();  // Print any failures that we have
  }
  void format(ItBase& it) override;

  void format_failure_messages();
  void prep_failure(ItBase& it);
};

/** @brief An assistant function for prep_failure to reduce complexity */
inline std::string Progress::prep_failure_helper(ItBase& it) {
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
  Description* parent = it.get_parent_as<Description>();

  do {
    helper_formatter.format(*parent);  // Format the node
    push_and_clear();
  } while ((parent = dynamic_cast<Description*>(parent->get_parent())) != nullptr);

  return Util::join(list);  // squash the list of strings and return it.
}

inline void Progress::prep_failure(ItBase& it) {
  std::ostringstream string_builder;  // oss is used as the local string builder
  if (color_output) {
    string_builder << RED;  // if we're doing color, make it red
  }
  string_builder << "Test number " << test_counter << " failed:";  // Tell us what test # failed
  if (color_output) {
    string_builder << RESET;  // if we're doing color, reset the terminal
  }
  string_builder << prep_failure_helper(it);
  if (color_output) {
    string_builder << RED;
  }
  string_builder << Util::join(raw_failure_messages, "\n");
  if (color_output) {
    string_builder << RESET;
  }
  raw_failure_messages.clear();
  baked_failure_messages.push_back(string_builder.str());
}

inline void Progress::format(ItBase& it) {
  if (it.get_result().status()) {
    if (color_output) {
      out_stream << GREEN;
    }
    out_stream << ".";
  } else {
    if (color_output) {
      out_stream << RED;
    }
    out_stream << "F";
    prep_failure(it);
  }
  if (color_output) {
    out_stream << RESET;
  }
  out_stream << std::flush;
  get_and_increment_test_counter();
}

inline void Progress::format_failure_messages() {
  if (!baked_failure_messages.empty()) {  // If we have any failures to format
    out_stream << std::endl;

    out_stream << Util::join(baked_failure_messages,
                             "\n\n")  // separated by a blank line
               << std::endl;          // newline

    baked_failure_messages.clear();  // Finally, clear the failures list.
  }
}

static Progress progress;

}  // namespace CppSpec::Formatters

/** @file */
#pragma once
#include <regex>
#include <string>

#include "matchers/matcher_base.hpp"

namespace CppSpec::Matchers {

// A std::regex keeps no copy of its source, so a pattern given as text is named by that text; one given as a
// std::regex is named only as a regex.
inline std::string describe_pattern(const std::string& pattern) {
  return pattern.empty() ? std::string{"a regex"} : "\"" + pattern + "\"";
}

// The actual text is read through string_view_of(); a null C string is no text, so it fails negated as well as
// positive.
template <Util::is_string_like A>
class Match : public MatcherBase<A, std::regex> {
  std::string pattern_;

 public:
  explicit Match(Expectation<A>& expectation, std::string expected)
      : MatcherBase<A, std::regex>(expectation, std::regex(expected)), pattern_(std::move(expected)) {}

  explicit Match(Expectation<A>& expectation, std::regex expected)
      : MatcherBase<A, std::regex>(expectation, expected) {}

  std::string verb() override { return "match"; }
  std::string description() override { return verb() + " " + describe_pattern(pattern_); }
  std::string failure_message() override {
    return MatcherBase<A, std::regex>::failure_message() + no_text_to_search(this->actual());
  }
  std::string failure_message_when_negated() override {
    return MatcherBase<A, std::regex>::failure_message_when_negated() + no_text_to_search(this->actual());
  }

  bool match() override {
    const auto actual = string_view_of(this->actual());
    return actual && std::regex_match(actual->begin(), actual->end(), this->expected());
  }

  bool negated_match() override { return string_view_of(this->actual()) && !match(); }
};

template <Util::is_string_like A>
class MatchPartial : public MatcherBase<A, std::regex> {
  std::string pattern_;

 public:
  explicit MatchPartial(Expectation<A>& expectation, std::string expected)
      : MatcherBase<A, std::regex>(expectation, std::regex(expected)), pattern_(std::move(expected)) {}

  explicit MatchPartial(Expectation<A>& expectation, std::regex expected)
      : MatcherBase<A, std::regex>(expectation, expected) {}

  std::string description() override { return "partially match " + describe_pattern(pattern_); }
  std::string failure_message() override {
    return MatcherBase<A, std::regex>::failure_message() + no_text_to_search(this->actual());
  }
  std::string failure_message_when_negated() override {
    return MatcherBase<A, std::regex>::failure_message_when_negated() + no_text_to_search(this->actual());
  }

  // A partial match is a match anywhere in the string: a search, not a match of the whole.
  bool match() override {
    const auto actual = string_view_of(this->actual());
    return actual && std::regex_search(actual->begin(), actual->end(), this->expected());
  }

  bool negated_match() override { return string_view_of(this->actual()) && !match(); }
};

}  // namespace CppSpec::Matchers

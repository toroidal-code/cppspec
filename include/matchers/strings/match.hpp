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

template <typename A>
class Match : public MatcherBase<A, std::regex> {
  std::string pattern_;

 public:
  explicit Match(Expectation<A>& expectation, std::string expected)
      : MatcherBase<A, std::regex>(expectation, std::regex(expected)), pattern_(std::move(expected)) {}

  explicit Match(Expectation<A>& expectation, std::regex expected)
      : MatcherBase<A, std::regex>(expectation, expected) {}

  std::string verb() override { return "match"; }
  std::string description() override { return verb() + " " + describe_pattern(pattern_); }

  bool match() override {
    std::smatch temp_match;
    return std::regex_match(this->actual(), temp_match, this->expected());
  }
};

template <typename A>
class MatchPartial : public MatcherBase<A, std::regex> {
  std::string pattern_;

 public:
  explicit MatchPartial(Expectation<A>& expectation, std::string expected)
      : MatcherBase<A, std::regex>(expectation, std::regex(expected)), pattern_(std::move(expected)) {}

  explicit MatchPartial(Expectation<A>& expectation, std::regex expected)
      : MatcherBase<A, std::regex>(expectation, expected) {}

  std::string description() override { return "partially match " + describe_pattern(pattern_); }

  // A partial match is a match anywhere in the string: a search, not a match of the whole.
  bool match() override { return std::regex_search(this->actual(), this->expected()); }
};

}  // namespace CppSpec::Matchers

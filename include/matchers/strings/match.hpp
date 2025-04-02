/** @file */
#pragma once
#include <regex>
#include <string>

#include "matchers/matcher_base.hpp"


namespace CppSpec::Matchers {

template <typename A>
class Match : MatcherBase<A, std::regex> {
 public:
  explicit Match(Expectation<A> &expectation, std::string expected)
      : MatcherBase<A, std::regex>(expectation, std::regex(expected)) {}

  explicit Match(Expectation<A> &expectation, std::regex expected)
      : MatcherBase<A, std::regex>(expectation, expected) {}

  std::string verb() override { return "match"; }

  bool match() override {
    std::smatch temp_match;
    return std::regex_match(this->actual(), temp_match, this->expected());
  }
};

template <typename A>
class MatchPartial : public MatcherBase<A, std::regex> {
 public:
  explicit MatchPartial(Expectation<A> &expectation, std::string expected)
      : MatcherBase<A, std::regex>(expectation, std::regex(expected)) {}

  explicit MatchPartial(Expectation<A> &expectation, std::regex expected)
      : MatcherBase<A, std::regex>(expectation, expected) {}

  std::string description() override { return "partially match " + Pretty::to_word(this->expected()); }

  bool match() override { return std::regex_match(this->actual(), this->expected()); }
};

}  // namespace CppSpec::Matchers


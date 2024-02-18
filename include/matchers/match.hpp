/** @file */
#ifndef CPPSPEC_MATCHERS_MATCH_HPP
#define CPPSPEC_MATCHERS_MATCH_HPP
#pragma once
#include <string>
#include <regex>
#include "matcher_base.hpp"

namespace CppSpec::Matchers {

template <typename A>
class Match : MatcherBase<A, std::regex> {
 public:
  explicit Match(Expectation<A> &expectation,
                 std::string expected)
      : MatcherBase<A, std::regex>(expectation, std::regex(expected)) {}

  explicit Match(Expectation<A> &expectation, std::regex expected)
      : MatcherBase<A, std::regex>(expectation, expected) {}

  std::string description() override {
    return "match " + Pretty::to_word(this->get_expected());
  }

  bool match() override {
    std::smatch temp_match;
    return std::regex_match(this->get_actual(), temp_match,
                            this->get_expected());
  }
};

template <typename A>
class MatchPartial : public MatcherBase<A, std::regex> {
 public:
  explicit MatchPartial(Expectation<A> &expectation,
                        std::string expected)
      : MatcherBase<A, std::regex>(expectation, std::regex(expected)) {}

  explicit MatchPartial(Expectation<A> &expectation,
                        std::regex expected)
      : MatcherBase<A, std::regex>(expectation, expected) {}

  std::string description() override {
    return "partially match " + Pretty::to_word(this->get_expected());
  }

  bool match() override {
    return std::regex_match(this->get_actual(), this->get_expected());
  }
};

} // namespace CppSpec::Matchers


#endif  // CPPSPEC_MATCHERS_BE_NULLPTR_HPP

/** @file */
#ifndef CPPSPEC_MATCHERS_START_WITH_HPP
#define CPPSPEC_MATCHERS_START_WITH_HPP
#pragma once

#include <string>
#include <ranges>
#include "matcher_base.hpp"


namespace CppSpec::Matchers {

template <std::ranges::range A, std::ranges::range E>
class StartWith : public MatcherBase<A, E> {

 public:
  StartWith(Expectation <A> &expectation, E start)
      : MatcherBase<A, E>(expectation, start) { }

  std::string description() override {
    return "start with " + Pretty::to_word(this->expected);
  }

  bool match() override {
    A &actual = this->get_actual();
    E &expected = this->get_expected();
    return std::equal(expected.begin(), expected.end(), actual.begin());
  }
};

} // namespace CppSpec::Matchers

#endif  // CPPSPEC_MATCHERS_START_WITH_HPP

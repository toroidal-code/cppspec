/** @file */
#pragma once

#include <ranges>
#include <string>

#include "matchers/matcher_base.hpp"

namespace CppSpec::Matchers {

template <std::ranges::range A, std::ranges::range E>
class StartWith : public MatcherBase<A, E> {
 public:
  StartWith(Expectation<A>& expectation, E start) : MatcherBase<A, E>(expectation, start) {}

  std::string verb() override { return "start with"; }

  bool match() override {
    A& actual = this->actual();
    E& expected = this->expected();
    return std::equal(expected.begin(), expected.end(), actual.begin());
  }
};

}  // namespace CppSpec::Matchers

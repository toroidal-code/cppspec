/** @file */
#pragma once

#include <ranges>
#include <string>

#include "matchers/matcher_base.hpp"

namespace CppSpec::Matchers {

template <std::ranges::range A, std::ranges::range E>
class EndWith : public MatcherBase<A, E> {
 public:
  EndWith(Expectation<A> &expectation, E start) : MatcherBase<A, E>(expectation, start) {}

  std::string description() override { return "end with " + Pretty::to_word(this->expected()); }

  bool match() override {
    A &actual = this->actual();
    E &expected = this->expected();
    return std::equal(expected.rbegin(), expected.rend(), actual.rbegin());
  }
};

}  // namespace CppSpec::Matchers


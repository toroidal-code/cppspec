/** @file */
#ifndef CPPSPEC_MATCHERS_END_WITH_HPP
#define CPPSPEC_MATCHERS_END_WITH_HPP
#pragma once

#include <string>
#include <ranges>
#include "matcher_base.hpp"

namespace CppSpec {
namespace Matchers {

template <std::ranges::range A, std::ranges::range E>
class EndWith : public MatcherBase<A, E> {
 public:
 EndWith(Expectation <A> &expectation, E start)
     : MatcherBase<A, E>(expectation, start) { }

  std::string description() override {
    return "end with " + Pretty::to_word(this->expected);
  }

  bool match() override {
    A &actual = this->get_actual();
    E &expected = this->get_expected();
    return std::equal(expected.rbegin(), expected.rend(), actual.rbegin());
  }
};

}  // namespace Matchers
}  // namespace CppSpec
#endif  // CPPSPEC_MATCHERS_END_WITH_HPP

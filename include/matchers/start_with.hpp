/** @file */
#ifndef CPPSPEC_MATCHERS_START_WITH_HPP
#define CPPSPEC_MATCHERS_START_WITH_HPP
#pragma once

#include <string>
#include "matcher_base.hpp"

namespace CppSpec {
namespace Matchers {

template <typename A, typename E>
class StartWith : public MatcherBase<A, E> {
  static_assert(Util::is_iterable<A>::value && Util::is_iterable<E>::value,
                "Error! Trying to match starting items with a non-iterable "
                    "container");

 public:
  StartWith(Expectations::Expectation <A> &expectation, E start)
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

}  // namespace Matchers
}  // namespace CppSpec
#endif  // CPPSPEC_MATCHERS_START_WITH_HPP

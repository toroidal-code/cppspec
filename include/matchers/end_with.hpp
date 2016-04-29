/** @file */
#ifndef CPPSPEC_MATCHERS_END_WITH_HPP
#define CPPSPEC_MATCHERS_END_WITH_HPP
#pragma once

#include <string>
#include "matcher_base.hpp"

namespace CppSpec {
namespace Matchers {

template <typename A, typename E>
class EndWith : public MatcherBase<A, E> {
  static_assert(Util::is_iterable<A>::value && Util::is_iterable<E>::value,
                "Error! Trying to match starting items with a non-iterable "
                    "container");

 public:
 EndWith(Expectations::Expectation <A> &expectation, E start)
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

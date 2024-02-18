/** @file */
#ifndef CPPSPEC_MATCHERS_FAIL_HPP
#define CPPSPEC_MATCHERS_FAIL_HPP
#pragma once

#include <string>
#include "matcher_base.hpp"


namespace CppSpec::Matchers {

template <typename A>
class Fail : public MatcherBase<A, void *> {
 public:
  static_assert(std::is_same_v<A, Result>,
                ".fail must be matched against a Result.");
  explicit Fail(Expectation<A> &expectation)
      : MatcherBase<A, void *>(expectation, nullptr) {}

  std::enable_if_t<std::is_same_v<A, Result>, bool>match() {
    return not this->get_actual().get_status();
  }
};

template <typename A>
class FailWith : public Matchers::MatcherBase<A, std::string> {
 public:
  FailWith(Expectation<A> &expectation, std::string expected)
      : Matchers::MatcherBase<A, std::string>(expectation, expected) {}

  std::enable_if_t<std::is_same_v<A, Result>, bool>match();
};

template <typename A>
std::enable_if_t<std::is_same_v<A, Result>, bool>
FailWith<A>::match() {
  static_assert(std::is_same<A, Result>::value,
                ".fail_with must be matched against a Result.");
  return (not this->get_actual().get_status()) &&
         this->get_actual().get_message() == this->get_expected();
}

} // namespace CppSpec::Matchers

#endif  // CPPSPEC_FAIL_HPP

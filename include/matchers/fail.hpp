/** @file */
#ifndef CPPSPEC_MATCHERS_FAIL_HPP
#define CPPSPEC_MATCHERS_FAIL_HPP
#pragma once

#include <string>
#include "matcher_base.hpp"

namespace CppSpec {
namespace Matchers {

template <typename A>
class Fail : public BaseMatcher<A, void *> {
 public:
  static_assert(std::is_same<A, Result>::value,
                ".fail must be matched against a Result.");
  explicit Fail(Expectations::Expectation<A> &expectation)
      : BaseMatcher<A, void *>(expectation, nullptr) {}

  typename std::enable_if<std::is_same<A, Result>::value, bool>::type match() {
    return not this->get_actual().get_status();
  }
};

template <typename A>
class FailWith : public Matchers::BaseMatcher<A, std::string> {
 public:
  FailWith(Expectations::Expectation<A> &expectation, std::string expected)
      : Matchers::BaseMatcher<A, std::string>(expectation, expected) {}

  typename std::enable_if<std::is_same<A, Result>::value, bool>::type match();
};

template <typename A>
typename std::enable_if<std::is_same<A, Result>::value, bool>::type
FailWith<A>::match() {
  static_assert(std::is_same<A, Result>::value,
                ".fail_with must be matched against a Result.");
  return (not this->get_actual().get_status()) &&
         this->get_actual().get_message() == this->get_expected();
}

}  // namespace Matchers
}  // namespace CppSpec
#endif  // CPPSPEC_FAIL_HPP

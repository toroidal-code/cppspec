/** @file */
#ifndef CPPSPEC_MATCHERS_BE_HPP
#define CPPSPEC_MATCHERS_BE_HPP
#pragma once

#include <string>
#include "matcher_base.hpp"

namespace CppSpec {
namespace Matchers {

template <class D>
class BeHelpers {
 protected:
  std::string parenthesize(std::string string) { return "(" + string + ")"; }
  std::string expected_to_sentance() {
    return this->split_words(this->get_expected());
  }
};

/********************
 *    Be matchers    *
 ********************/

/**
 * @brief The basic `be` matcher
 */
template <typename A>
class Be : public BaseMatcher<A, bool>, BeHelpers<Be<A>> {
  std::function<bool(A)> test;

 public:
  Be(Expectations::Expectation<A> &expectation, std::function<bool(A)> test)
      : BaseMatcher<A, bool>(expectation), test(test) {}

  std::string description() override;
  std::string failure_message() override;
  std::string failure_message_when_negated() override;

  bool match() override;
};

template <typename A>
std::string Be<A>::description() {
  std::stringstream ss;
  ss << "be" << Pretty::to_sentance(this->expected);
  return ss.str();
}

template <typename A>
std::string Be<A>::failure_message() {
  std::stringstream ss;
  ss << "expected " << BaseMatcher<A, bool>::get_actual()
     << " to evaluate to true";
  return ss.str();
}

template <typename A>
std::string Be<A>::failure_message_when_negated() {
  std::stringstream ss;
  ss << "expected " << BaseMatcher<A, bool>::get_actual()
     << " to evaluate to false";
  return ss.str();
}

template <typename A>
bool Be<A>::match() {
  return test(this->get_actual());
}

template <class A>
class BeNullptr : BaseMatcher<A, std::nullptr_t> {
 public:
  explicit BeNullptr(Expectations::Expectation<A> &expectation)
      : BaseMatcher<A, std::nullptr_t>(expectation) {}

  std::string failure_message() override;
  std::string failure_message_when_negated() override;

  bool match() override;
};

template <class T>
std::string BeNullptr<T>::failure_message() {
  return "expected: nullptr\n     got: " + BaseMatcher<T, T>::get_actual();
}

template <class T>
std::string BeNullptr<T>::failure_message_when_negated() {
  return "expected: not nullptr\n     got: nullptr";
}

template <class A>
bool BeNullptr<A>::match() {
  return this->get_actual() == nullptr;
}

}  // namespace Matchers
}  // namespace CppSpec
#endif  // CPPSPEC_MATCHERS_BE_HPP

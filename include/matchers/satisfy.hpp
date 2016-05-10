/** @file */
#ifndef CPPSPEC_MATCHERS_SATISFY_HPP
#define CPPSPEC_MATCHERS_SATISFY_HPP
#pragma once

#include <string>
#include "matcher_base.hpp"

namespace CppSpec {
namespace Matchers {
//
//template <class D>
//class BeHelpers {
// protected:
//  std::string parenthesize(std::string string) { return "(" + string + ")"; }
//  std::string expected_to_sentance() {
//    return this->split_words(this->get_expected());
//  }
//};

template <typename A>
class Satisfy : public MatcherBase<A, bool> //, BeHelpers<Satisfy<A>>
{
  std::function<bool(A)> test;

 public:
  Satisfy(Expectation<A> &expectation, std::function<bool(A)> test)
      : MatcherBase<A, bool>(expectation), test(test) {}

  std::string description() override;
  std::string failure_message() override;
  std::string failure_message_when_negated() override;

  bool match() override;
};

template <typename A>
std::string Satisfy<A>::description() {
  std::stringstream ss;
  ss << "be" << Pretty::to_sentance(this->expected);
  return ss.str();
}

template <typename A>
std::string Satisfy<A>::failure_message() {
  std::stringstream ss;
  ss << "expected " << MatcherBase<A, bool>::get_actual()
     << " to evaluate to true";
  return ss.str();
}

template <typename A>
std::string Satisfy<A>::failure_message_when_negated() {
  std::stringstream ss;
  ss << "expected " << MatcherBase<A, bool>::get_actual()
     << " to evaluate to false";
  return ss.str();
}

template <typename A>
bool Satisfy<A>::match() {
  return test(this->get_actual());
}

}  // namespace Matchers
}  // namespace CppSpec
#endif  // CPPSPEC_MATCHERS_SATISFY_HPP

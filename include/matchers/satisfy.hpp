/** @file */
#pragma once

#include <string>

#include "matcher_base.hpp"

namespace CppSpec::Matchers {
//
// template <class D>
// class BeHelpers {
// protected:
//  std::string parenthesize(std::string string) { return "(" + string + ")"; }
//  std::string expected_to_sentence() {
//    return this->split_words(this->expected());
//  }
//};

template <typename A>
class Satisfy : public MatcherBase<A, bool>  //, BeHelpers<Satisfy<A>>
{
  std::function<bool(A)> test;

 public:
  Satisfy(Expectation<A> &expectation, std::function<bool(A)> test) : MatcherBase<A, bool>(expectation), test(test) {}

  std::string failure_message() override;
  std::string failure_message_when_negated() override;
  std::string verb() override { return "be"; }

  bool match() override;
};

template <typename A>
std::string Satisfy<A>::failure_message() {
  return std::format("expected {} to evaluate to true", MatcherBase<A, bool>::actual());
}

template <typename A>
std::string Satisfy<A>::failure_message_when_negated() {
  return std::format("expected {} to evaluate to false", MatcherBase<A, bool>::actual());
}

template <typename A>
bool Satisfy<A>::match() {
  return test(this->actual());
}

}  // namespace CppSpec::Matchers


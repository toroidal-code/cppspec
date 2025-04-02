/** @file */
#pragma once
#include <string>

#include "matcher_base.hpp"


namespace CppSpec::Matchers {

template <class A>
class BeNullptr : MatcherBase<A, std::nullptr_t> {
 public:
  explicit BeNullptr(Expectation<A> &expectation) : MatcherBase<A, std::nullptr_t>(expectation) {}

  std::string verb() override { return "be"; }
  std::string description() override { return "be nullptr"; }
  bool match() override { return this->actual() == nullptr; }
};

}  // namespace CppSpec::Matchers


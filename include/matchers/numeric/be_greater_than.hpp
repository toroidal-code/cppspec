/** @file */
#pragma once
#include <string>

#include "matchers/matcher_base.hpp"


namespace CppSpec::Matchers {

template <typename A, typename E>
class BeGreaterThan : public MatcherBase<A, E> {
 public:
  BeGreaterThan(Expectation<A> &expectation, E expected) : MatcherBase<A, E>(expectation, expected) {}
  bool match() override { return this->actual() > this->expected(); }
  std::string verb() override { return "be greater than"; }
};

}  // namespace CppSpec::Matchers


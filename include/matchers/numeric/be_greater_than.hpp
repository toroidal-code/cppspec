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
  std::string description() override { return "be greater than" + Pretty::to_word(this->expected()); }
};

}  // namespace CppSpec::Matchers


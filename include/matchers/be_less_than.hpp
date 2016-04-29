/** @file */
#ifndef CPPSPEC_MATCHERS_BE_LESS_THAN_HPP
#define CPPSPEC_MATCHERS_BE_LESS_THAN_HPP
#pragma once
#include <string>
#include "matcher_base.hpp"
namespace CppSpec {
namespace Matchers {

template <typename A, typename E>
class BeLessThan : public MatcherBase<A, E> {
 public:
  BeLessThan(Expectations::Expectation<A> &expectation, E expected)
      : MatcherBase<A, E>(expectation, expected) {}
  bool match() override { return this->get_actual() < this->get_expected(); }
  std::string description() override {
    return "be less than" + Pretty::to_word(this->get_expected());
  }
};

}  // namespace Matchers
}  // namespace CppSpec

#endif  // CPPSPEC_MATCHERS_BE_LESS_THAN_HPP

/** @file */
#ifndef CPPSPEC_MATCHERS_BE_LESS_THAN_HPP
#define CPPSPEC_MATCHERS_BE_LESS_THAN_HPP
#pragma once
#include <string>
#include "matcher_base.hpp"

namespace CppSpec::Matchers {

template <typename A, typename E>
class BeLessThan : public MatcherBase<A, E> {
 public:
  BeLessThan(Expectation<A> &expectation, E expected)
      : MatcherBase<A, E>(expectation, expected) {}
  bool match() override { return this->get_actual() < this->get_expected(); }
  std::string description() override {
    return "be less than" + Pretty::to_word(this->get_expected());
  }
};

} // namespace CppSpec::Matchers


#endif  // CPPSPEC_MATCHERS_BE_LESS_THAN_HPP

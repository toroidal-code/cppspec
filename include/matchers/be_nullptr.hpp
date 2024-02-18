/** @file */
#ifndef CPPSPEC_MATCHERS_BE_NULLPTR_HPP
#define CPPSPEC_MATCHERS_BE_NULLPTR_HPP
#pragma once
#include <string>
#include "matcher_base.hpp"

namespace CppSpec::Matchers {

template <class A>
class BeNullptr : MatcherBase<A, std::nullptr_t> {
 public:
  explicit BeNullptr(Expectation<A> &expectation)
      : MatcherBase<A, std::nullptr_t>(expectation) {}

  std::string description() override { return "be nullptr"; }
  bool match() override { return this->get_actual() == nullptr; }
};

} // namespace CppSpec::Matchers


#endif  // CPPSPEC_MATCHERS_BE_NULLPTR_HPP

#pragma once

#include "matchers/matcher_base.hpp"

namespace CppSpec::Matchers {

template <typename A>
class Fail : public MatcherBase<A, void*> {
 public:
  static_assert(is_result_v<A>, ".fail must() be matched against a Result.");
  explicit Fail(Expectation<A>& expectation) : MatcherBase<A, void*>(expectation, nullptr) {}

  bool match() { return !this->actual().status(); }
};

template <typename A>
class FailWith : public MatcherBase<A, std::string> {
 public:
  static_assert(is_result_v<A>, ".fail_with() must be matched against a Result.");
  FailWith(Expectation<A>& expectation, std::string expected) : MatcherBase<A, std::string>(expectation, expected) {}

  bool match() { return (!this->actual().status()) && this->actual().get_message() == this->expected(); }
};

}  // namespace CppSpec::Matchers

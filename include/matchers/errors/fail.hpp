#pragma once

#include "matchers/matcher_base.hpp"

namespace CppSpec::Matchers {

template <typename A>
class Fail : public MatcherBase<A, void*> {
 public:
  static_assert(is_result_v<A>, ".fail must() be matched against a Matcher.");
  explicit Fail(Expectation<A>& expectation) : MatcherBase<A, void*>(expectation, nullptr) {}
  std::string verb() override { return "fail"; }
  bool match() override { return this->actual().is_failure(); }
};

template <typename A>
class FailWith : public MatcherBase<A, std::string> {
 public:
  static_assert(is_result_v<A>, ".fail_with() must be matched against a Result.");
  FailWith(Expectation<A>& expectation, std::string expected) : MatcherBase<A, std::string>(expectation, expected) {}

  std::string verb() override { return "fail with"; }
  std::string description() override { return std::format(R"(fail with "{}")", this->expected()); }

  bool match() override {
    auto message = this->actual().get_message();
    return this->actual().is_failure() && message == this->expected();
  }
};

}  // namespace CppSpec::Matchers

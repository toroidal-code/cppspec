#pragma once

#include "matchers/matcher_base.hpp"
#include "util.hpp"

namespace CppSpec::Matchers {

template <class A, class Ex>
class Throw : public MatcherBase<A, void*> {
 public:
  explicit Throw(Expectation<A>& expectation) : MatcherBase<A, void*>(expectation, nullptr) {}
  bool match() override;
  std::string verb() override { return "throw"; }
  std::string description() override;
  std::string failure_message() override;
  std::string failure_message_when_negated() override;
};

template <class A, class Ex>
bool Throw<A, Ex>::match() {
  bool caught = false;
  try {
    this->actual();
  } catch (Ex& ex) {
    caught = true;
  }
  return caught;
}

template <typename A, typename Ex>
std::string Throw<A, Ex>::description() {
  return std::format("throw {}", Util::demangle(typeid(Ex).name()));
}

template <typename A, typename Ex>
std::string Throw<A, Ex>::failure_message() {
  return std::format("expected the given function ([] -> {} {{...}}) to {}", Util::demangle(typeid(A).name()),
                     description());
}

template <typename A, typename Ex>
std::string Throw<A, Ex>::failure_message_when_negated() {
  return std::format("expected the given function ([] -> {} {{...}}) not to {}", Util::demangle(typeid(A).name()),
                     description());
}
}  // namespace CppSpec::Matchers

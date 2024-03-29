/** @file */
#pragma once

#include <functional>
#include <type_traits>

#include "matchers/matcher_base.hpp"
#include "util.hpp"

namespace CppSpec::Matchers {

template <class A, class Ex>
class Throw : public MatcherBase<A, void *> {
 public:
  explicit Throw(Expectation<A> &expectation) : MatcherBase<A, void *>(expectation, nullptr) {}
  bool match() override;
  std::string description() override;
  std::string failure_message() override;
  std::string failure_message_when_negated() override;
};

template <class A, class Ex>
bool Throw<A, Ex>::match() {
  bool caught = false;
  try {
    this->actual();
  } catch (Ex &ex) {
    caught = true;
  }
  return caught;
}

template <typename A, typename Ex>
std::string Throw<A, Ex>::description() {
  std::stringstream ss;
  ss << "throw " << Util::demangle(typeid(Ex).name());
  return ss.str();
}

template <typename A, typename Ex>
std::string Throw<A, Ex>::failure_message() {
  std::stringstream ss;
  ss << "expected the given function ([] -> " << Util::demangle(typeid(A).name()) << " {...}) to "
     << this->description();
  return ss.str();
}

template <typename A, typename Ex>
std::string Throw<A, Ex>::failure_message_when_negated() {
  std::stringstream ss;
  ss << "expected the given function ([] -> " << Util::demangle(typeid(A).name()) << " {...}) not to "
     << this->description();
  return ss.str();
}

}  // namespace CppSpec::Matchers


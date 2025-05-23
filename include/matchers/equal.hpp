/** @file */
#pragma once

#include <string>

#include "matcher_base.hpp"

namespace CppSpec::Matchers {

/**
 * @brief The `equal` matcher
 *
 * @tparam A the type of the actual value
 * @tparam E the type of the expected value
 */
template <typename A, typename E>
class Equal : public MatcherBase<A, E> {
 public:
  Equal(Expectation<A>& expectation, E expected) : MatcherBase<A, E>(expectation, expected) {}

  std::string verb() override { return "equal"; }
  std::string failure_message() override;
  std::string failure_message_when_negated() override;
  bool diffable();

 protected:
  bool match() override;
  bool expected_is_a_literal();
  std::string actual_inspected();
  std::string simple_failure_message();
  // std::string detailed_failure_message();
};

template <typename A, typename E>
std::string Equal<A, E>::failure_message() {
  // if (expected_is_a_literal()) {
  //   return simple_failure_message();
  // } else {
  //   return detailed_failure_message();
  // }
  return simple_failure_message();
}

template <typename A, typename E>
std::string Equal<A, E>::failure_message_when_negated() {
  std::stringstream ss;
  ss << "expected not " << Pretty::inspect_object(MatcherBase<A, E>::expected()) << std::endl;
  ss << "         got " << actual_inspected() << std::endl;
  ss << "Compared using `==`";
  return ss.str();
}

template <typename A, typename E>
std::string Equal<A, E>::simple_failure_message() {
  std::stringstream ss;
  ss << "expected " << Pretty::inspect_object(MatcherBase<A, E>::expected()) << std::endl;
  ss << "     got " << actual_inspected() << std::endl;
  ss << "Compared using `==`";
  return ss.str();
}

template <typename A, typename E>
bool Equal<A, E>::diffable() {
  return !expected_is_a_literal();
}

template <typename A, typename E>
bool Equal<A, E>::match() {
  return this->expected() == this->actual();
}

template <typename A, typename E>
bool Equal<A, E>::expected_is_a_literal() {
  return (typeid(E) == typeid(bool));
}

// template <typename E>
// std::string Equal<bool, E>::actual_inspected() {
//   std::stringstream ss;
//   ss << std::boolalpha << BaseMatcher<bool,E>::actual();
//   return ss.str();
// }

template <typename A, typename E>
std::string Equal<A, E>::actual_inspected() {
  return Pretty::inspect_object(MatcherBase<A, E>::actual());
}

// template <typename A, bool E>
// std::string Equal<A, bool>::simple_failure_message() {
//   std::stringstream ss;
//   ss << "\nexpected " << std::boolalpha <<
//   BaseMatcher<A,bool>::expected() << "\n"
//       "     got " << Equal<A,bool>::actual_inspected() << "\n";
//   return ss.str();
// }

// template <>
// std::string Equal<bool>::inspect_object(bool o) {
//   std::stringstream ss;
//   ss << "#<#" << Util::demangle(typeid(o).name()) <<
//       "> => " << std::boolalpha <<  o;
//   return ss.str();
// }

}  // namespace CppSpec::Matchers

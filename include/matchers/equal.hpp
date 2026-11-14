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

  /**
   * @brief Whether the two values are compared as text, through string_view_of(), whatever form each is in, so two
   *        C strings are compared by their text and not by their addresses
   */
  static constexpr bool compares_text = Util::is_string_like<A> && Util::is_string_like<E>;

  std::string verb() override { return "equal"; }
  std::string failure_message() override;
  std::string failure_message_when_negated() override;
  bool diffable();

 protected:
  bool match() override;
  bool expected_is_a_literal();
  std::string comparison();
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
  ss << comparison();
  return ss.str();
}

template <typename A, typename E>
std::string Equal<A, E>::simple_failure_message() {
  std::stringstream ss;
  ss << "expected " << Pretty::inspect_object(MatcherBase<A, E>::expected()) << std::endl;
  ss << "     got " << actual_inspected() << std::endl;
  ss << comparison();
  return ss.str();
}

template <typename A, typename E>
bool Equal<A, E>::diffable() {
  return !expected_is_a_literal();
}

// A null C string is a value of its own: it equals another null C string and differs from every piece of text.
template <typename A, typename E>
bool Equal<A, E>::match() {
  if constexpr (compares_text) {
    return string_view_of(this->actual()) == string_view_of(this->expected());
  } else {
    return this->expected() == this->actual();
  }
}

template <typename A, typename E>
std::string Equal<A, E>::comparison() {
  if constexpr (compares_text) {
    if (!string_view_of(this->actual()) || !string_view_of(this->expected())) {
      return "A null C string equals only another null C string";
    }
  }
  return "Compared using `==`";
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

#ifndef EQUAL_H
#define EQUAL_H
#include <string>
#include <sstream>
#include <typeinfo>
#include "basematcher.hpp"
#include "../util.hpp"

namespace Matchers {

/**
 * @brief The `equal` matcher
 *
 * @tparam A the type of the actual value
 * @tparam E the type of the expected value
 */
template <typename A, typename E>
class Equal : public BaseMatcher<A, E> {
 public:
  Equal(Expectations::Expectation<A> expectation)
      : BaseMatcher<A, E>(expectation) {}

  Equal(Expectations::Expectation<A> expectation, E expected)
      : BaseMatcher<A, E>(expectation, expected) {}

  virtual std::string description();
  std::string failure_message();
  std::string failure_message_when_negated();
  bool diffable();

 protected:
  bool match(E expected, A actual);
  bool expected_is_a_literal();
  std::string actual_inspected();
  std::string simple_failure_message();
  // std::string detailed_failure_message();
};

template <typename A, typename E>
std::string Equal<A, E>::description() {
  std::stringstream ss;
  ss << "equal" << Pretty::to_sentance(this->expected);
  return ss.str();
}

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
  ss << "\n"
        "expected not "
     << Util::inspect_object(BaseMatcher<A, E>::get_actual()) << "\n"
                                                                 "         got "
     << Util::inspect_object(BaseMatcher<A, E>::get_expected())
     << "\n"
        "\n"
        "Compared using `==`.\n\n";
  return ss.str();
}

template <typename A, typename E>
std::string Equal<A, E>::simple_failure_message() {
  std::stringstream ss;
  ss << "\nexpected " << Util::inspect_object(BaseMatcher<A, E>::get_expected())
     << "\n"
        "     got "
     << actual_inspected() << "\n";
  return ss.str();
}

template <typename A, typename E>
bool Equal<A, E>::diffable() {
  return !expected_is_a_literal();
}

// TODO: match should pass by reference
template <typename A, typename E>
bool Equal<A, E>::match(E expected, A actual) {
  // TODO: compare pointers too
  return expected == actual;
}

template <typename A, typename E>
bool Equal<A, E>::expected_is_a_literal() {
  return (typeid(E) == typeid(bool));
}

// template <typename E>
// std::string Equal<bool, E>::actual_inspected() {
//   std::stringstream ss;
//   ss << std::boolalpha << BaseMatcher<bool,E>::get_actual();
//   return ss.str();
// }

template <typename A, typename E>
std::string Equal<A, E>::actual_inspected() {
  return Util::inspect_object(BaseMatcher<A, E>::get_actual());
}

// template <typename A, bool E>
// std::string Equal<A, bool>::simple_failure_message() {
//   std::stringstream ss;
//   ss << "\nexpected " << std::boolalpha <<
//   BaseMatcher<A,bool>::get_expected() << "\n"
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
}

#endif /* EQUAL_H */

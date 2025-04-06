#pragma once

#include "matchers/matcher_base.hpp"

namespace CppSpec::Matchers {
template <typename A, typename E>
class BeWithin;

template <typename A, typename E>
class BeWithinHelper {
  Expectation<A>& expectation;
  E tolerance;
  std::string msg;

 public:
  BeWithinHelper(Expectation<A>& expectation, E tolerance) : expectation(expectation), tolerance(tolerance) {}

  BeWithin<A, E> of(E expected);
  BeWithin<A, E> percent_of(E expected);
  void set_message(const std::string& msg) { this->msg = msg; }
  std::string get_message() { return this->msg; }
};

template <typename A, typename E>
class BeWithin : public MatcherBase<A, E> {
  E tolerance;
  std::string unit;

 public:
  BeWithin(Expectation<A>& expectation, E tolerance, E value, std::string_view unit)
      : MatcherBase<A, E>(expectation, value), tolerance{tolerance}, unit{unit} {}

  bool match() override;

  std::string failure_message() override;
  std::string failure_message_when_negated() override;
  std::string description() override;
  std::string verb() override { return "be within"; }
};

template <typename A, typename E>
BeWithin<A, E> BeWithinHelper<A, E>::of(E expected) {
  auto matcher = BeWithin<A, E>(expectation, tolerance, expected, "");  // No unit specified
  matcher.set_message(msg);
  return matcher;
}

template <typename A, typename E>
BeWithin<A, E> BeWithinHelper<A, E>::percent_of(E expected) {
  auto matcher = BeWithin<A, E>(expectation, tolerance, expected, "%");  // Percent unit specified
  matcher.set_message(msg);
  return matcher;
}

template <typename A, typename E>
bool BeWithin<A, E>::match() {
  if (!this->expected()) {
    return false;
  }
  return std::abs(this->actual() - this->expected()) <= this->tolerance;
}

template <typename A, typename E>
std::string BeWithin<A, E>::failure_message() {
  return std::format("expected {} to {}", this->actual(), description());
}

template <typename A, typename E>
std::string BeWithin<A, E>::failure_message_when_negated() {
  return std::format("expected {} not to {}", this->actual(), description());
}

template <typename A, typename E>
std::string BeWithin<A, E>::description() {
  return std::format("be within {}{} of {}", this->tolerance, this->unit, this->expected());
}

}  // namespace CppSpec::Matchers

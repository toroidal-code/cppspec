/** @file */
#pragma once

#include "matchers/matcher_base.hpp"

namespace CppSpec::Matchers {

template <typename A, typename E>
class BeWithin : public MatcherBase<A, E> {
  E delta;
  std::string unit;
  E tolerance;

 public:
  BeWithin(Expectation<A> &expectation, E delta) : MatcherBase<A, E>(expectation, 0), delta(delta) {}

  bool of(E expected);
  bool percent_of(E expected);

  bool match() override;

  std::string failure_message() override;
  std::string failure_message_when_negated() override;
  std::string description() override;
};

template <typename A, typename E>
bool BeWithin<A, E>::of(E expected) {
  this->expected_ = expected;
  this->tolerance = this->delta;
  this->unit = "";
  return this->run(this->get_formatter());
}

template <typename A, typename E>
bool BeWithin<A, E>::percent_of(E expected) {
  this->expected_ = expected;
  this->tolerance = this->delta;
  this->unit = "%";
  return this->run(this->get_formatter());
}

template <typename A, typename E>
bool BeWithin<A, E>::match() {
  if (!this->expected()) {
    std::stringstream ss;
    ss << "You must set an expected value using #of: be_within(" << this->delta << ").of(expected_value)";

    return false;
  }
  return std::abs(this->actual() - this->expected()) <= this->tolerance;
}

template <typename A, typename E>
std::string BeWithin<A, E>::failure_message() {
  std::stringstream ss;
  ss << "expected " << this->actual() << " to " << description();
  return ss.str();
}

template <typename A, typename E>
std::string BeWithin<A, E>::failure_message_when_negated() {
  std::stringstream ss;
  ss << "expected " << this->actual() << " not to " << description();
  return ss.str();
}

template <typename A, typename E>
std::string BeWithin<A, E>::description() {
  std::stringstream ss;
  ss << "be within " << this->delta << this->unit << " of " << this->expected();
  return ss.str();
}

}  // namespace CppSpec::Matchers


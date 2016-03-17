#ifndef BE_WITHIN_H
#define BE_WITHIN_H
#include <complex>
#include <sstream>
#include "basematcher.hpp"

namespace Matchers {

template <typename A, typename E>
class BeWithin : public BaseMatcher<A, E> {
  E delta;
  std::string unit;
  E tolerance;

 public:
  BeWithin(Expectations::Expectation<A> expectation, E delta)
      : BaseMatcher<A, E>(expectation, 0), delta(delta) {}

  BeWithin& of(E expected);
  BeWithin& percent_of(E expected);

  bool matches(A actual);

  std::string failure_message();
  std::string failure_message_when_negated();
  std::string description();
};

template <typename A, typename E>
BeWithin<A, E>& BeWithin<A, E>::of(E expected) {
  this->expected = expected;
  this->tolerance = this->delta;
  this->unit = "";
  return *this;
}

template <typename A, typename E>
BeWithin<A, E>& BeWithin<A, E>::percent_of(E expected) {
  this->expected = expected;
  this->tolerance = this->delta;
  this->unit = "%";
  return *this;
}

template <typename A, typename E>
bool BeWithin<A, E>::matches(A actual) {
  this->actual = actual;
  if (!this->expected) {
    std::cout << RED << "You must set an expected value using #of: be_within("
              << this->delta << ").of(expected_value)" << RESET << std::endl;
    return false;
  }
  return std::abs(this->actual - this->expected) <= this->tolerance;
}

template <typename A, typename E>
std::string BeWithin<A, E>::failure_message() {
  std::stringstream ss;
  ss << "expected " << this->actual << " to " << description();
  return ss.str();
}

template <typename A, typename E>
std::string BeWithin<A, E>::failure_message_when_negated() {
  std::stringstream ss;
  ss << "expected " << this->actual << " not to " << description();
  return ss.str();
}

template <typename A, typename E>
std::string BeWithin<A, E>::description() {
  std::stringstream ss;
  ss << "be within " << this->delta << this->unit << " of " << this->expected;
  return ss.str();
}
}

#endif /* BE_WITHIN_H */

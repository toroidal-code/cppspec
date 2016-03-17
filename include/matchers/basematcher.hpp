#ifndef BASEMATCHER_H
#define BASEMATCHER_H

#include <string>
#include <sstream>
#include <iostream>
#include <memory>
#include "pretty_matchers.hpp"
#include "../runnable.hpp"
#include "../expectations/handler.hpp"
#include "../it_base.hpp"

namespace Expectations {
template <class T>
class Expectation;
}

using namespace Expectations;

namespace Matchers {
template <typename Actual, typename Expected>
class BaseMatcher : public Child, public Pretty {
 protected:
  Actual actual;
  Expected expected;
  Expectation<Actual> expectation;

 public:
  BaseMatcher(Expectation<Actual> &expectation) : expectation(expectation) {
    // static_assert(std::is_same<Expected,Actual>::value,
    //               "Error: Expected value's and actual value's and types are
    //               different");
  }
  BaseMatcher(Expectation<Actual> &expectation, Expected expected)
      : expected(expected), expectation(expectation) {
    // static_assert(std::is_same<Expected,Actual>::value,
    //               "Error: Expected value's and actual value's and types are
    //               different");
  }

  virtual ~BaseMatcher(){};

  virtual bool matches(Actual actual);
  virtual std::string failure_message();
  virtual std::string failure_message_when_negated();
  virtual std::string description();
  Actual get_actual() { return actual; }
  Expected get_expected() { return expected; }
  virtual bool match(Expected, Actual) { return false; }
  bool operator()(std::string message = "");
};

template <typename A, typename E>
bool BaseMatcher<A, E>::matches(A actual) {
  this->actual = actual;
  return match(expected, actual);
}

template <typename A, typename E>
std::string BaseMatcher<A, E>::failure_message() {
  std::stringstream ss;
  ss << "expected " << actual << " to " << description();
  return ss.str();
}

template <typename A, typename E>
std::string BaseMatcher<A, E>::failure_message_when_negated() {
  std::stringstream ss;
  ss << "expected " << actual << " to " << description();
  return ss.str();
}

template <typename A, typename E>
std::string BaseMatcher<A, E>::description() {
  std::stringstream ss;
  ss << this->name_to_sentance() << this->to_sentance(expected);
  return ss.str();
}

template <typename A, typename E>
bool BaseMatcher<A, E>::operator()(std::string message) {
  bool matched;
  ItBase *par = static_cast<ItBase *>(this->get_parent());
  if (par->needs_descr()) {
    std::cout << par->padding() << "should " << this->description()
              << std::endl;
  }
  if (expectation.get_sign()) {
    matched = PositiveExpectationHandler::handle_matcher<A>(
        expectation.get_target(), *this, message);
  } else {
    matched = NegativeExpectationHandler::handle_matcher<A>(
        expectation.get_target(), *this, message);
  }

  // if our items didn't match, we obviously failed.
  if (!matched) this->failed();

  return matched;
}
}

#endif /* BASEMATCHER_H */

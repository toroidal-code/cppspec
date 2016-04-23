#ifndef HANDLER_H
#define HANDLER_H
#include "printer_base.hpp"

namespace Expectations {

struct PositiveExpectationHandler {
  template <typename A, class Matcher>
  static Result handle_matcher(Matcher &matcher);
  static std::string verb() { return "should"; }
};

struct NegativeExpectationHandler {
  template <typename A, class Matcher>
  static Result handle_matcher(Matcher &matcher);
  static std::string verb() { return "should not"; }
};

template <typename A, class Matcher>
Result PositiveExpectationHandler::handle_matcher(Matcher &matcher) {
  // TODO: handle expectation failure here
  return !matcher.match() ? Result::failure_with(matcher.failure_message()) : Result::success;
}

template <typename A, class Matcher>
Result NegativeExpectationHandler::handle_matcher(Matcher &matcher) {
  // TODO: handle expectation failure here
  return !matcher.negated_match() ? Result::failure_with(matcher.failure_message_when_negated())
                                  : Result::success;
}
}  // Expectations

#endif /* HANDLER_H */

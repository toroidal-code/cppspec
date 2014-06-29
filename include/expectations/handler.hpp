#ifndef HANDLER_H
#define HANDLER_H
#include <string>
#include "matchers/basematcher.hpp"
#include "fail_with.hpp"
//#include "matchers/negativebasematcher.hpp"

namespace Expectations {

namespace ExpectationHelper {

template <class M>
void handle_failure_common(M /*matcher*/, std::string message) {
  // if (matcher.diffable()) {
  //   Expectations::fail_with(message, matcher.get_expected(),
  //   matcher.get_actual());
  // }
  Expectations::fail_with(message);
}

template <typename Matcher>
void handle_failure(Matcher &matcher, std::string message) {
  if (message.empty()) {
    message = matcher.failure_message();
  }
  handle_failure_common(matcher, message);
}

template <typename Matcher>
void handle_negated_failure(Matcher &matcher, std::string message) {
  if (message.empty()) {
    message = matcher.failure_message_when_negated();
  }
  handle_failure_common(matcher, message);
}
}

// Common ancestor. Literally just a stub

struct NegativeExpectationHandler;

struct PositiveExpectationHandler {
  template <typename A, class Matcher>
  static bool handle_matcher(A actual, Matcher &matcher, std::string messsage);
  static std::string verb() { return "should"; }
};

struct NegativeExpectationHandler {
  template <typename A, class Matcher>
  static bool handle_matcher(A actual, Matcher &matcher, std::string messsage);
  static std::string verb() { return "should not"; }
  template <typename A, class Matcher>
  static bool does_not_match(Matcher &matcher, A actual);
  // static bool does_not_match(NegativeBaseMatcher<T> matcher, T actual); //
  // negative matchers have #does_not_match defined
};

template <typename A, class Matcher>
bool PositiveExpectationHandler::handle_matcher(A actual, Matcher &matcher,
                                                std::string message) {
  // TODO: handle expectation failure here
  bool matched = matcher.matches(actual);
  if (!matched) {
    ExpectationHelper::handle_failure(matcher, message);
  }
  return matched;
}

template <typename A, class Matcher>
bool NegativeExpectationHandler::handle_matcher(A actual, Matcher &matcher,
                                                std::string message) {
  // TODO: handle expectation failure here
  bool matched = does_not_match(matcher, actual);
  if (!matched) {
    ExpectationHelper::handle_negated_failure(matcher, message);
  }
  return matched;
}

template <typename A, class Matcher>
bool NegativeExpectationHandler::does_not_match(Matcher &matcher, A actual) {
  return !matcher.matches(actual);
}

}  // Expectations

#endif /* HANDLER_H */

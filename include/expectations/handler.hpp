#ifndef HANDLER_H
#define HANDLER_H
#include "fail_with.hpp"
//#include "matchers/negativebasematcher.hpp"

namespace Expectations {

namespace ExpectationHelper {

template <class M>
void handle_failure_common(M & /*matcher*/, std::string message) {
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

struct PositiveExpectationHandler {
  template <typename A, class Matcher>
  static bool handle_matcher(Matcher &matcher, std::string message);
  static std::string verb() { return "should"; }
};

struct NegativeExpectationHandler {
  template <typename A, class Matcher>
  static bool handle_matcher(Matcher &matcher, std::string message);
  static std::string verb() { return "should not"; }
};

template <typename A, class Matcher>
bool PositiveExpectationHandler::handle_matcher(Matcher &matcher,
                                                std::string message) {
  // TODO: handle expectation failure here
  bool matched = matcher.match();
  if (!matched) {
    ExpectationHelper::handle_failure(matcher, message);
  }
  return matched;
}

template <typename A, class Matcher>
bool NegativeExpectationHandler::handle_matcher(Matcher &matcher,
                                                std::string message) {
  // TODO: handle expectation failure here
  bool matched = matcher.negated_match();
  if (!matched) {
    ExpectationHelper::handle_negated_failure(matcher, message);
  }
  return matched;
}
}  // Expectations

#endif /* HANDLER_H */

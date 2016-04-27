/** @file */
#ifndef CPPSPEC_MATCHERS_MATCHER_BASE_HPP
#define CPPSPEC_MATCHERS_MATCHER_BASE_HPP
#pragma once

#include <string>
#include "pretty_matchers.hpp"
#include "expectations/handler.hpp"
#include "it_base.hpp"

namespace CppSpec {

namespace Expectations {
template <class T>
class Expectation;
}

namespace Matchers {

template <typename Actual, typename Expected>
class BaseMatcher : public Runnable, public Pretty {
  std::string custom_failure_message = "";

 protected:
  Expected expected;
  Expectations::Expectation<Actual> expectation;

 public:
  BaseMatcher(BaseMatcher<Actual, Expected> const &copy)
      : Runnable(copy.get_parent()),
        custom_failure_message(copy.custom_failure_message),
        expected(copy.expected),
        expectation(copy.expectation) {}

  explicit BaseMatcher(Expectations::Expectation<Actual> &expectation)
      : Runnable(*expectation.get_parent()),  // We want the parent of the
                                              // matcher to be the `it` block,
                                              // not the
                                              // Expectation.
        expectation(expectation) {}

  BaseMatcher(Expectations::Expectation<Actual> &expectation, Expected expected)
      : Runnable(*expectation.get_parent()),
        expected(expected),
        expectation(expectation) {}

  // TODO: match and negated match should return Result
  virtual bool match() = 0;
  virtual bool negated_match() { return !match(); }
  virtual std::string failure_message();
  virtual std::string failure_message_when_negated();
  virtual std::string description();
  Actual &get_actual() { return expectation.get_target(); }
  Expected &get_expected() { return expected; }
  Expectations::Expectation<Actual> &get_expectation() { return expectation; }
  virtual BaseMatcher &set_message(std::string message);
  Result run(Formatters::BaseFormatter &printer) override;
  typedef Expected expected_t;
};

template <typename A, typename E>
BaseMatcher<A, E> &BaseMatcher<A, E>::set_message(std::string message) {
  this->custom_failure_message = message;
  return *this;
}

template <typename A, typename E>
std::string BaseMatcher<A, E>::failure_message() {
  if (not custom_failure_message.empty()) {
    return this->custom_failure_message;
  } else {
    std::stringstream ss;
    ss << "expected" << Pretty::to_sentance(get_actual()) << " to " << description();
    return ss.str();
  }
}

template <typename A, typename E>
std::string BaseMatcher<A, E>::failure_message_when_negated() {
  if (not custom_failure_message.empty()) {
    return this->custom_failure_message;
  } else {
    std::stringstream ss;
    ss << "expected" << Pretty::to_sentance(get_actual()) << " to not " << description();
    return ss.str();
  }
}

template <typename A, typename E>
std::string BaseMatcher<A, E>::description() {
  std::stringstream ss;
  std::string pretty_expected = this->to_sentance(expected);
//  ss << "match " << this->name_to_sentance(Util::demangle(typeid(*this).name()))
//     << "(" << pretty_expected.substr(1, pretty_expected.length()) << ")";
  ss << "match" << Pretty::to_sentance(expected);
  return ss.str();
}

template <typename A, typename E>
Result BaseMatcher<A, E>::run(Formatters::BaseFormatter &printer) {
  BaseIt *par = static_cast<BaseIt *>(this->get_parent());
  // If we need a description for our test, generate it
  // unless we're ignoring the output.
  if (par->needs_descr() && !expectation.get_ignore_failure()) {
    std::stringstream ss;
    ss << (expectation.get_sign()
               ? Expectations::PositiveExpectationHandler::verb()
               : Expectations::NegativeExpectationHandler::verb())
       << " " << this->description();
    std::string ss_str = ss.str();
    par->set_descr(ss_str);
  }

  Result matched =
      expectation.get_sign()
          ? Expectations::PositiveExpectationHandler::handle_matcher<A>(*this)
          : Expectations::NegativeExpectationHandler::handle_matcher<A>(*this);

  // If our items didn't match, we obviously failed.
  // Only report the failure if we aren't actively ignoring it.
  if (!matched && !expectation.get_ignore_failure()) {
    this->failed();
    std::string message = matched.get_message();
    if (message.empty()) {
      printer.format_failure(
          "Failure message is empty. Does your matcher define the "
          "appropriate failure_message[_when_negated] method to "
          "return a string?");
    } else {
      printer.format_failure(matched.get_message());
    }
  }
  //return !expectation.get_ignore_failure() ? matched : Result::success();
  return matched;
}

}  // namespace Matchers
}  // namespace CppSpec
#endif  // CPPSPEC_MATCHERS_MATCHER_BASE_HPP

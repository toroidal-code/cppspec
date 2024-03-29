/**
 * Copyright 2016 Katherine Whitlock
 *
 * @file matcher_base.hpp
 * @brief Contains the base class for all Matchers
 *
 * @author Katherine Whitlock (toroidalcode)
 */

#pragma once

#include <string>

#include "expectations/handler.hpp"
#include "formatters/formatters_base.hpp"
#include "it_base.hpp"
#include "pretty_matchers.hpp"

namespace CppSpec {

template <class T>
class Expectation;  // Forward declaration of Expectation

namespace Matchers {

/**
 * @brief the base class for all Matcher classes and objects
 *
 * @tparam Actual the type of Actual 'thing' to match against
 *                in the context of `expect(2)`, it would be `int`
 *
 * @tparam Expected the type of Expected 'thing' to match for
 *                  in the contet of `expect(2).to_equal(2.0)`,
 *                  it would be `float`
 */
template <typename Actual, typename Expected>
class MatcherBase : public Runnable, public Pretty {
  std::string custom_failure_message;

 protected:
  Expected expected_;  // The expected object contained by the matcher

  // A reference to the Expectation (i.e. `expect(2)`)
  Expectation<Actual> &expectation_;

 public:
  // Copy constructor
  MatcherBase(MatcherBase<Actual, Expected> const &copy) = default;

  // Constructor when matcher has no 'object' to match for
  explicit MatcherBase(Expectation<Actual> &expectation)
      : Runnable(*expectation.get_parent()),  // We want the parent of the
                                              // matcher to be the `it` block,
                                              // not the
                                              // Expectation.
        expectation_(expectation) {}

  // Constructor when the matcher has an object to match for. This is the most
  // commonly used constructor
  MatcherBase(Expectation<Actual> &expectation, Expected expected)
      : Runnable(*expectation.get_parent()), expected_(expected), expectation_(expectation) {}

  /*--------- Helper functions -------------*/

  virtual std::string failure_message();
  virtual std::string failure_message_when_negated();
  virtual std::string description();

  // Get the 'actual' object from the Expectation
  constexpr Actual &actual() { return expectation_.get_target(); }

  // Get the 'expected' object from the Matcher
  Expected &expected() { return expected_; }

  // Get the Expectation itself
  Expectation<Actual> &expectation() { return expectation_; }

  // Set the message to give on match failure
  virtual MatcherBase &set_message(const std::string &message);

  /*--------- Primary functions -------------*/

  // Run the matcher
  Result run(Formatters::BaseFormatter &printer) override;

  // TODO: match and negated match should return Result
  virtual bool match() = 0;
  virtual bool negated_match() { return !match(); }

  // typedef Expected
  using expected_t = Expected;
};

/**
 * @brief Set a custom failure message
 *
 * @param message the message to give
 * @return the modified Matcher
 */
template <typename A, typename E>
MatcherBase<A, E> &MatcherBase<A, E>::set_message(const std::string &message) {
  this->custom_failure_message = message;
  return *this;
}

/**
 * @brief Get message to give on match failure
 *
 * @return the message
 */
template <typename A, typename E>
std::string MatcherBase<A, E>::failure_message() {
  if (not custom_failure_message.empty()) {
    return this->custom_failure_message;
  }
  std::stringstream ss;
  ss << "expected " << Pretty::to_word(actual()) << " to " << description();
  return ss.str();
}

/**
 * @brief Get message to give on match failure when negated
 *
 * @return the message
 */
template <typename A, typename E>
std::string MatcherBase<A, E>::failure_message_when_negated() {
  if (not custom_failure_message.empty()) {
    return this->custom_failure_message;
  }
  std::stringstream ss;
  ss << "expected " << Pretty::to_word(actual()) << " to not " << description();
  return ss.str();
}

/**
 * @brief Get the description of the Matcher
 *
 * @return the description
 */
template <typename A, typename E>
std::string MatcherBase<A, E>::description() {
  std::stringstream ss;
  //std::string pretty_expected = this->to_sentence(expected_);
  //  ss << "match " <<
  //  this->name_to_sentence(Util::demangle(typeid(*this).name()))
  //     << "(" << pretty_expected.substr(1, pretty_expected.length()) << ")";
  ss << "match" << Pretty::to_sentence(expected_);
  return ss.str();
}

/**
 * @brief Run the Matcher object
 *
 * @param printer the formatter to print using
 *
 * @return the Result of running the Matcher
 */
template <typename A, typename E>
Result MatcherBase<A, E>::run(Formatters::BaseFormatter &printer) {
  auto *par = static_cast<ItBase *>(this->get_parent());
  // If we need a description for our test, generate it
  // unless we're ignoring the output.
  if (par->needs_description() && !expectation_.ignore_failure()) {
    std::stringstream ss;
    ss << (expectation_.sign() ? PositiveExpectationHandler::verb() : NegativeExpectationHandler::verb()) << " "
       << this->description();
    std::string ss_str = ss.str();
    par->set_description(ss_str);
  }

  Result matched = expectation_.sign() ? PositiveExpectationHandler::handle_matcher(*this)
                                       : NegativeExpectationHandler::handle_matcher(*this);

  // If our items didn't match, we obviously failed.
  // Only report the failure if we aren't actively ignoring it.
  if (!matched && !expectation_.ignore_failure()) {
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
  return matched;
}

}  // namespace Matchers
}  // namespace CppSpec

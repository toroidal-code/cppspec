/**
 * Copyright 2016 Katherine Whitlock
 *
 * @file matcher_base.hpp
 * @brief Contains the base class for all Matchers
 *
 * @author Katherine Whitlock (toroidalcode)
 */

#pragma once

#include <source_location>
#include <string>
#include <string_view>

#include "expectations/handler.hpp"
#include "it_base.hpp"
#include "pretty_matchers.hpp"

namespace CppSpec {

template <class T>
class Expectation;  // Forward declaration of Expectation

namespace Matchers {

/**
 * @brief The line a matcher that searches text puts below its own message when the actual value is a null C string,
 *        which has no text to search, so the matcher fails it negated as well as positive
 *
 * @param actual the actual value
 * @return the line, led by a newline, or an empty string when the actual value is text
 */
template <typename A>
[[nodiscard]] std::string no_text_to_search(const A& actual) {
  if constexpr (Util::is_string_like<A>) {
    if (!string_view_of(actual)) {
      return "\nnullptr is not text, so there is nothing to search";
    }
  }
  return {};
}

/**
 * @brief The line a matcher that searches text puts below its own message when the text or pattern it looks for is
 *        a null C string, which is nothing to look for, so the matcher fails it negated as well as positive
 */
inline constexpr std::string_view nothing_to_look_for = "\nnullptr is not text, so there is nothing to look for";

/**
 * @brief The line nothing_to_look_for when the expected value is a null C string
 *
 * @param expected the expected value
 * @return the line, led by a newline, or an empty string when the expected value is not a null C string
 */
template <typename E>
[[nodiscard]] std::string no_text_to_look_for(const E& expected) {
  if constexpr (Util::is_string_like<E>) {
    if (!string_view_of(expected)) {
      return std::string(nothing_to_look_for);
    }
  }
  return {};
}

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
class MatcherBase : public Pretty {
  std::string custom_failure_message;

  // The custom message, when one was set, on the line above the matcher's own. An empty own message is replaced by
  // a diagnostic here, so a custom message never sits above a blank line.
  [[nodiscard]] std::string with_custom_message(std::string own) const {
    if (own.empty()) {
      own = empty_failure_message;
    }
    return custom_failure_message.empty() ? own : custom_failure_message + "\n" + own;
  }

 protected:
  Expected expected_;  // The expected object contained by the matcher

  // A reference to the Expectation (i.e. `expect(2)`)
  Expectation<Actual>& expectation_;

 public:
  // Copy constructor
  MatcherBase(MatcherBase<Actual, Expected> const& copy) = default;

  // Constructor when matcher has no 'object' to match for
  explicit MatcherBase(Expectation<Actual>& expectation)
      // We want the parent of the matcher to be the `it` block, not the Expectation.
      : expectation_(expectation) {}

  // Constructor when the matcher has an object to match for. This is the most
  // commonly used constructor
  MatcherBase(Expectation<Actual>& expectation, Expected expected) : expected_(expected), expectation_(expectation) {}

  virtual ~MatcherBase() = default;

  /*--------- Helper functions -------------*/

  virtual std::string failure_message();
  virtual std::string failure_message_when_negated();
  virtual std::string description();
  virtual std::string verb() { return "match"; }

  // Get the 'actual' object from the Expectation
  constexpr Actual& actual() { return expectation_.get_target(); }

  // Get the 'expected' object from the Matcher
  Expected& expected() { return expected_; }

  // Get the Expectation itself
  Expectation<Actual>& expectation() { return expectation_; }

  // Set the message to give on match failure
  virtual MatcherBase& set_message(std::string message);

  /**
   * @brief What a failed match reports: the custom message, when one was set, on the line above the matcher's own
   *        failure_message()
   */
  [[nodiscard]] std::string full_failure_message() { return with_custom_message(failure_message()); }

  /**
   * @brief What a failed negated match reports: the custom message, when one was set, on the line above the
   *        matcher's own failure_message_when_negated()
   */
  [[nodiscard]] std::string full_failure_message_when_negated() {
    return with_custom_message(failure_message_when_negated());
  }

  /** @brief What a failure reports in place of a matcher's own message that is empty */
  static constexpr std::string_view empty_failure_message =
      "Failure message is empty. Does your matcher define the appropriate failure_message[_when_negated] method to "
      "return a string?";

  [[nodiscard]] std::source_location get_location() const { return expectation_.get_location(); }

  /*--------- Primary functions -------------*/

  // Run the matcher
  Result run();

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
MatcherBase<A, E>& MatcherBase<A, E>::set_message(std::string message) {
  this->custom_failure_message = std::move(message);
  return *this;
}

/**
 * @brief Get the matcher's own message for a failed match; the handler puts any custom message above it
 *
 * @return the message
 */
template <typename A, typename E>
std::string MatcherBase<A, E>::failure_message() {
  return std::format("expected {} to {}", Pretty::to_word(actual()), description());
}

/**
 * @brief Get the matcher's own message for a failed negated match; the handler puts any custom message above it
 *
 * @return the message
 */
template <typename A, typename E>
std::string MatcherBase<A, E>::failure_message_when_negated() {
  return std::format("expected {} to not {}", Pretty::to_word(actual()), description());
}

/**
 * @brief Get the description of the Matcher
 *
 * @return the description
 */
template <typename A, typename E>
std::string MatcherBase<A, E>::description() {
  // std::string pretty_expected = this->to_sentence(expected_);
  //   ss << "match " <<
  //   this->name_to_sentence(Util::demangle(typeid(*this).name()))
  //      << "(" << pretty_expected.substr(1, pretty_expected.length()) << ")";
  // to_sentence() leads with the space that separates it from the verb.
  return std::format("{}{}", verb(), Pretty::to_sentence(expected_));
}

/**
 * @brief Run the Matcher object
 *
 * @param printer the formatter to print using
 *
 * @return the Result of running the Matcher
 */
template <typename A, typename E>
Result MatcherBase<A, E>::run() {
  Result result = expectation_.positive() ? PositiveExpectationHandler::handle_matcher(*this)
                                          : NegativeExpectationHandler::handle_matcher(*this);

  result.set_type(Util::demangle(typeid(*this).name()));

  // If our items didn't match, we obviously failed.
  // Only report the failure if we aren't actively ignoring it.
  // A handler that does not build its message with full_failure_message*() can still report an empty one.
  if (result.is_failure() && result.get_message().empty()) {
    result.set_message(std::string(empty_failure_message));
  }

  if (expectation_.ignored()) {
    result.set_status(Result::Status::Skipped);
  }

  ItBase* parent = expectation_.get_it();
  if (parent != nullptr) {
    // If we need a description for our test, generate it
    // unless we're ignoring the output.
    if (parent->needs_description() && !expectation_.ignored()) {
      parent->set_description(
          (expectation_.positive() ? PositiveExpectationHandler::verb() : NegativeExpectationHandler::verb()) + " " +
          this->description());
    }
    parent->add_result(result);
  }
  return result;
}

}  // namespace Matchers
}  // namespace CppSpec

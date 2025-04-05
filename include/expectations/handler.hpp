/**
 * Copyright 2016 Katherine Whitlock
 *
 * @file handler.hpp
 * @brief Contains the primary handlers for running Matchers
 *
 * @author Katherine Whitlock (toroidalcode)
 */

#pragma once

#include <exception>
#include <string>

#include "result.hpp"

namespace CppSpec {

/** @brief Handles "positive" expectations (i.e. non-negated) */
struct PositiveExpectationHandler {
  template <class Matcher>
  static Result handle_matcher(Matcher& matcher);
  static std::string verb() { return "should"; }
};

/** @brief Handles "negative" expectations (i.e. negated with '.not_() */
struct NegativeExpectationHandler {
  template <class Matcher>
  static Result handle_matcher(Matcher& matcher);
  static std::string verb() { return "should not"; }
};

/**
 * @brief runs a positive expectation
 *
 * @tparam Matcher the class of Matcher to use
 * @param matcher the matcher to use
 *
 * @return the Result of the expectation
 */
template <class Matcher>
Result PositiveExpectationHandler::handle_matcher(Matcher& matcher) {
  bool matched = false;
  try {
    matched = matcher.match();
  } catch (std::exception& e) {
    return Result::error_with(matcher.get_location(), e.what());
  } catch (...) {
    return Result::error_with(matcher.get_location(), "Unknown exception thrown during matcher execution.");
  }

  return !matched ? Result::failure_with(matcher.get_location(), matcher.failure_message())
                  : Result::success(matcher.get_location());
}

/**
 * @brief runs a negative expectation
 *
 * @tparam Matcher the class of Matcher to use
 * @param matcher the matcher to use
 *
 * @return the Result of the expectation
 */
template <class Matcher>
Result NegativeExpectationHandler::handle_matcher(Matcher& matcher) {
  bool matched = false;
  try {
    matched = matcher.negated_match();
  } catch (std::exception& e) {
    return Result::error_with(matcher.get_location(), e.what());
  } catch (...) {
    return Result::error_with(matcher.get_location(), "Unhandled exception thrown during matcher execution.");
  }
  return !matched ? Result::failure_with(matcher.get_location(), matcher.failure_message_when_negated())
                  : Result::success(matcher.get_location());
}

}  // namespace CppSpec

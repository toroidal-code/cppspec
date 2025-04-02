/**
 * @file
 * @brief Defines the Expectation class and associated functions
 */

#pragma once

#include <exception>
#include <regex>
#include <string>
#include <vector>

#include "matchers/be_nullptr.hpp"
#include "matchers/contain.hpp"
#include "matchers/equal.hpp"
#include "matchers/errors/fail.hpp"
#include "matchers/errors/have_value.hpp"
#include "matchers/errors/throw.hpp"
#include "matchers/numeric/be_between.hpp"
#include "matchers/numeric/be_greater_than.hpp"
#include "matchers/numeric/be_less_than.hpp"
#include "matchers/numeric/be_within.hpp"
#include "matchers/satisfy.hpp"
#include "matchers/strings/end_with.hpp"
#include "matchers/strings/match.hpp"
#include "matchers/strings/start_with.hpp"

#ifdef __cpp_lib_expected
#include "matchers/errors/have_error.hpp"
#endif

namespace CppSpec {

/**
 * @brief Wraps the target of an expectation
 * @code
 *   expect(something)
 *   expect([] -> auto { return something })
 *
 *   // used with a matcher
 *   expect(actual).to_equal(target)
 *
 *   // used with `not` and a matcher
 *   expect(actual).not_().to_equal(target)
 * @endcode
 *
 * Static typing forces us to either give the type of the expected result
 * to the Matcher object, or chain in a builder-like fashion.
 */
template <class A>
class Expectation : public Child {
 protected:
  bool is_positive_ = true;
  // Have we been negated?
  bool ignore_failure_ = false;

 public:
  /**
   * @brief Create an Expectation using a value.
   *
   * @param value The target to test, an explicit value.
   *
   * @return The constructed Expectation.
   */
  explicit Expectation(ItBase &it) : Child(Child::of(it)) {}

  /** @brief Get the target of the expectation. */
  // virtual const A &get_target() const & { return target; }
  virtual A &get_target() & = 0;

  /** @brief Get whether the expectation is normal or negated. */
  [[nodiscard]] constexpr bool sign() const { return is_positive_; }
  [[nodiscard]] constexpr bool ignore_failure() const { return ignore_failure_; }

  /********* Modifiers *********/

  virtual Expectation &not_() = 0;
  virtual Expectation &ignore() = 0;

  /********* Matchers *********/

  template <class M>
  Result to(M matcher, std::string msg = "");

  /*-------- to be... ----------*/

  Result to_be_false(std::string msg = "");
  Result to_be_falsy(std::string msg = "");
  Result to_be_null(std::string msg = "");
  Result to_be_true(std::string msg = "");
  Result to_be_truthy(std::string msg = "");

  template <typename E>
  Result to_be_between(E min, E max, Matchers::RangeMode mode = Matchers::RangeMode::inclusive, std::string msg = "");

  template <typename E>
  Result to_be_greater_than(E rhs, std::string msg = "");

  template <typename E>
  Result to_be_less_than(E rhs, std::string msg = "");

  template <typename E>
  Matchers::BeWithinHelper<A, E> to_be_within(E expected, std::string msg = "");

  /*-------- to... ----------*/

  Result to_end_with(std::string ending, std::string msg = "");
  Result to_fail(std::string msg = "");
  Result to_fail_with(std::string failure_message, std::string msg = "");
  Result to_match(std::regex regex, std::string msg = "");
  Result to_match(std::string str, std::string msg = "");
  Result to_partially_match(std::regex regex, std::string msg = "");
  Result to_partially_match(std::string str, std::string msg = "");
  Result to_satisfy(std::function<bool(A)> /*test*/, std::string msg = "");
  Result to_start_with(std::string start, std::string msg = "");

  template <typename U>
  Result to_contain(std::initializer_list<U> expected, std::string msg = "");

  template <typename E>
  Result to_contain(E expected, std::string msg = "");

  template <typename U>
  Result to_end_with(std::initializer_list<U> start, std::string msg = "");

  template <typename E>
  Result to_equal(E expected, std::string msg = "");

  template <typename U>
  Result to_start_with(std::initializer_list<U> start, std::string msg = "");

  Result to_have_value(std::string msg = "");
#if __cpp_lib_expected
  Result to_have_error(std::string msg = "");
#endif
};

/**
 *
 *
 * @param matcher
 * @param msg
 * @tparam A
 * @tparam M
 *
 * @return
 */
template <typename A>
template <class M>
Result Expectation<A>::to(M matcher, std::string msg) {
  static_assert(std::is_base_of_v<Matchers::MatcherBase<A, typename M::expected_t>, M>,
                "Matcher is not a subclass of BaseMatcher.");
  // auto base_matcher = static_cast<Matchers::BaseMatcher<A,typename
  // M::expected_t>>(matcher);
  return matcher.set_message(msg).run(this->get_formatter());
}

/**
 * @brief Match using the Matchers::Be matcher, testing for falsy-ness.
 *
 * @param msg Optional message to give on failure.
 * @tparam A
 *
 * @return
 */
template <typename A>
Result Expectation<A>::to_be_false(std::string msg) {
  static_assert(std::is_same_v<A, bool>,
                ".to_be_false() can only be used on booleans or functions that return booleans");
  return to_equal(false, msg);
}

/**
 *
 *
 * @param msg
 *
 * @return
 */
template <typename A>
Result Expectation<A>::to_be_falsy(std::string msg) {
  return to_satisfy([](const A &t) { return !static_cast<bool>(t); }, msg);
}

/**
 * @brief Match using the Matchers::BeNullptr matcher.
 *
 * @param msg Optional message to give on failure.
 *
 * @return
 */
template <typename A>
Result Expectation<A>::to_be_null(std::string msg) {
  return Matchers::BeNullptr<A>(*this).set_message(msg).run(this->get_formatter());
}

/**
 * @brief Match using the Matchers::Be matcher, testing for truthy-ness.
 *
 * @param msg Optional message to give on failure.
 *
 * @return
 */
template <typename A>
Result Expectation<A>::to_be_true(std::string msg) {
  static_assert(std::is_same_v<A, bool>,
                ".to_be_true() can only be used on booleans or functions that return booleans");
  // return to_be([](A t) { return static_cast<bool>(t); }, msg);
  return to_equal(true, msg);
}

/**
 *
 *
 * @param msg
 *
 * @tparam A
 * @return
 */
template <typename A>
Result Expectation<A>::to_be_truthy(std::string msg) {
  return to_satisfy([](const A &t) { return static_cast<bool>(t); }, msg);
}

/**
 * @brief Match using the Matchers::BeBetween matcher, with an explicit
 *        range mode.
 *
 * @param min
 * @param max
 * @param msg Optional message to give on failure.
 *
 * @return
 */
template <typename A>
template <typename E>
Result Expectation<A>::to_be_between(E min, E max, Matchers::RangeMode mode, std::string msg) {
  return Matchers::BeBetween<A, E>(*this, min, max, mode).set_message(msg).run(this->get_formatter());
}

template <typename A>
template <typename E>
Result Expectation<A>::to_be_less_than(E rhs, std::string msg) {
  return Matchers::BeLessThan<A, E>(*this, rhs).set_message(msg).run(this->get_formatter());
}

template <typename A>
template <typename E>
Result Expectation<A>::to_be_greater_than(E rhs, std::string msg) {
  return Matchers::BeGreaterThan<A, E>(*this, rhs).set_message(msg).run(this->get_formatter());
}

/**
 * @brief Match using the Matchers::Include matcher, given an initializer list.
 *
 * @param expected
 * @param msg Optional message to give on failure.
 *
 * @return
 */
template <typename A>
template <typename U>
Result Expectation<A>::to_contain(std::initializer_list<U> expected, std::string msg) {
  return Matchers::Contain<A, std::vector<U>, U>(*this, expected).set_message(msg).run(this->get_formatter());
}

/**
 * @brief Match using the Matchers::Include matcher.
 *
 * @param expected
 * @param msg Optional message to give on failure.
 *
 * @return
 */
template <typename A>
template <typename E>
Result Expectation<A>::to_contain(E expected, std::string msg) {
  return Matchers::Contain<A, E, E>(*this, expected).set_message(msg).run(this->get_formatter());
}

/**
 * @brief Match using the Matchers::Equal matcher.
 *
 * @param expected
 * @param msg Optional message to give on failure.
 *
 * @return
 */
template <typename A>
template <typename E>
Result Expectation<A>::to_equal(E expected, std::string msg) {
  return Matchers::Equal<A, E>(*this, expected).set_message(msg).run(this->get_formatter());
}

/**
 * @brief Match using the Matchers::BeWithin matcher.
 *
 * @param expected
 * @param msg Optional message to give on failure.
 *
 * @return
 */
template <typename A>
template <typename E>
Matchers::BeWithinHelper<A, E> Expectation<A>::to_be_within(E expected, std::string msg) {
  Matchers::BeWithinHelper<A, E> matcher(*this, expected);
  matcher.set_message(msg);
  return matcher;
}

template <typename A>
Result Expectation<A>::to_fail(std::string msg) {
  static_assert(is_result_v<A>, ".to_fail() must be used on an expression that returns a Result.");
  return Matchers::Fail<Result>(*this).set_message(msg).run(this->get_formatter());
}

template <typename A>
Result Expectation<A>::to_fail_with(std::string failure_message, std::string msg) {
  static_assert(is_result_v<A>, ".to_fail_with() must be used on an expression that returns a Result.");
  return Matchers::FailWith<A>(*this, failure_message).set_message(msg).run(this->get_formatter());
}

template <typename A>
Result Expectation<A>::to_match(std::string str, std::string msg) {
  return Matchers::Match<A>(*this, str).set_message(msg).run(this->get_formatter());
}

template <typename A>
Result Expectation<A>::to_match(std::regex regex, std::string msg) {
  return Matchers::Match<A>(*this, regex).set_message(msg).run(this->get_formatter());
}

template <typename A>
Result Expectation<A>::to_partially_match(std::string str, std::string msg) {
  return Matchers::MatchPartial<A>(*this, str).set_message(msg).run(this->get_formatter());
}

template <typename A>
Result Expectation<A>::to_partially_match(std::regex regex, std::string msg) {
  return Matchers::MatchPartial<A>(*this, regex).set_message(msg).run(this->get_formatter());
}

/**
 * @brief Match using the Matchers::Satisfy matcher.
 *
 * @param test The function to use to test the output of the
 *             expectation expression.
 * @param msg Optional message to give on failure.
 *
 * @return Whether the expectation succeeds or fails.
 */
template <typename A>
Result Expectation<A>::to_satisfy(std::function<bool(A)> test, std::string msg) {
  return Matchers::Satisfy<A>(*this, test).set_message(msg).run(this->get_formatter());
}

template <typename A>
Result Expectation<A>::to_start_with(std::string start, std::string msg) {
  return Matchers::StartWith<std::string, std::string>(*this, start).set_message(msg).run(this->get_formatter());
}

template <typename A>
template <typename U>
Result Expectation<A>::to_start_with(std::initializer_list<U> start_sequence, std::string msg) {
  return Matchers::StartWith<A, std::initializer_list<U>>(*this, start_sequence)
      .set_message(msg)
      .run(this->get_formatter());
}

template <typename A>
Result Expectation<A>::to_end_with(std::string ending, std::string msg) {
  return Matchers::EndWith<std::string, std::string>(*this, ending).set_message(msg).run(this->get_formatter());
}

template <typename A>
template <typename U>
Result Expectation<A>::to_end_with(std::initializer_list<U> start_sequence, std::string msg) {
  return Matchers::StartWith<A, std::initializer_list<U>>(*this, start_sequence)
      .set_message(msg)
      .run(this->get_formatter());
}

template <typename A>
Result Expectation<A>::to_have_value(std::string msg) {
  return Matchers::HaveValue<A>(*this).set_message(msg).run(this->get_formatter());
}

#if __cpp_lib_expected
template <typename A>
Result Expectation<A>::to_have_error(std::string msg) {
  return Matchers::HaveError<A>(*this).set_message(msg).run(this->get_formatter());
}
#endif

template <typename A>
class ExpectationValue : public Expectation<A> {
  A value;
  std::source_location location;

 public:
  /**
   * @brief Create an ExpectationValue using a value.
   *
   * @param value The target to test, an explicit value.
   *
   * @return The constructed ExpectationValue.
   */
  ExpectationValue(ItBase &it, A value, std::source_location location)
      : Expectation<A>(it), value(value), location{location} {}

  /**
   * @brief Create an Expectation using an initializer list.
   *
   * @param init_list The initializer list to match against.
   *
   * @return The constructed Expectation.
   */
  template <typename U>
  ExpectationValue(ItBase &it, std::initializer_list<U> init_list, std::source_location location)
      : Expectation<A>(it), value(std::vector<U>(init_list)), location{location} {}

  /** @brief Get the target of the expectation. */
  A &get_target() & override { return value; }

  ExpectationValue &not_() override {
    this->is_positive_ = not this->is_positive_;
    return *this;
  }

  ExpectationValue &ignore() override {
    this->ignore_failure_ = true;
    return *this;
  }
};

template <Util::is_functional F>
class ExpectationFunc : public Expectation<decltype(std::declval<F>()())> {
  using block_ret_t = decltype(std::declval<F>()());
  F block;
  std::shared_ptr<block_ret_t> computed = nullptr;
  std::source_location location;

 public:
  ExpectationFunc(ExpectationFunc<F> const &copy, std::source_location location) : Expectation<block_ret_t>(copy), block(copy.block), location{location} {}

  /**
   * @brief Create an ExpectationValue using a value.
   *
   * @param value The target to test, an explicit value.
   *
   * @return The constructed ExpectationValue.
   */
  ExpectationFunc(ItBase &it, F block, std::source_location location) : Expectation<block_ret_t>(it), block(block), location{location} {}

  /**
   * @brief Create an Expectation using a function.
   *
   * This does not simply contain the return value of the given
   * lambda, but instead wraps the thunk, delaying execution until it is time
   * to perform the match.
   *
   * @param block A function that returns some value to test against.
   *
   * @return The constructed Expectation.
   */
  // TODO: create a "lazy" parameter for differentiating between delayed and
  // immediate execution
  //  ExpectationFunc(BaseIt &it, std::function<A()> block)
  //      : Expectation(it), block(block()){};  // block(block), has_block(true)
  //      {}

  /** @brief Get the target of the expectation. */
  block_ret_t &get_target() & override {
    if (computed == nullptr) {
      computed = std::make_shared<block_ret_t>(block());
    }
    return *computed;
  }

  // auto get_target() &  override -> decltype(std::declval<A>()()) & { return
  // Expectation<A>::get_target()(); }

  ExpectationFunc &not_() override {
    this->is_positive_ = !this->is_positive_;
    return *this;
  }

  ExpectationFunc &ignore() override {
    this->ignore_failure_ = true;
    return *this;
  }

  Expectation<decltype(block())> &casted() { return static_cast<decltype(block())>(*this); }

  template <typename Ex = std::exception>
  Result to_throw(std::string msg = "");
};

template <Util::is_functional F>
template <typename Ex>
Result ExpectationFunc<F>::to_throw(std::string msg) {
  Matchers::Throw<decltype(this->block.operator()()), Ex> matcher(*this);
  return matcher.set_message(msg).run(this->get_formatter());
}

}  // namespace CppSpec

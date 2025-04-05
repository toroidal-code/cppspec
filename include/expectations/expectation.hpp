#pragma once

#include <exception>
#include <regex>
#include <source_location>
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
class Expectation {
  ItBase* it = nullptr;
  std::source_location location;

 protected:
  bool is_positive_ = true;
  // Have we been negated?
  bool ignore_ = false;

 public:
  Expectation() = default;
  explicit Expectation(std::source_location location) : location(location) {}

  /**
   * @brief Create an Expectation using a value.
   *
   * @param value The target to test, an explicit value.
   *
   * @return The constructed Expectation.
   */
  explicit Expectation(ItBase& it, std::source_location location) : it(&it), location(location) {}

  /** @brief Get the target of the expectation. */
  // virtual const A &get_target() const & { return target; }
  virtual A& get_target() & = 0;

  [[nodiscard]] ItBase* get_it() const { return it; }
  [[nodiscard]] std::source_location get_location() const { return location; }

  /** @brief Get whether the expectation is normal or negated. */
  [[nodiscard]] constexpr bool positive() const { return is_positive_; }
  [[nodiscard]] constexpr bool ignored() const { return ignore_; }

  /********* Modifiers *********/

  virtual Expectation& not_() = 0;
  virtual Expectation& ignore() = 0;

  /********* Matchers *********/

  template <class M>
  void to(M matcher, std::string msg = "");

  /*-------- to be... ----------*/

  void to_be_false(std::string msg = "");
  void to_be_falsy(std::string msg = "");
  void to_be_null(std::string msg = "");
  void to_be_true(std::string msg = "");
  void to_be_truthy(std::string msg = "");

  template <typename E>
  void to_be_between(E min, E max, Matchers::RangeMode mode = Matchers::RangeMode::inclusive, std::string msg = "");

  template <typename E>
  void to_be_greater_than(E rhs, std::string msg = "");

  template <typename E>
  void to_be_less_than(E rhs, std::string msg = "");

  template <typename E>
  Matchers::BeWithinHelper<A, E> to_be_within(E expected, std::string msg = "");

  /*-------- to... ----------*/

  void to_end_with(std::string ending, std::string msg = "");
  void to_fail(std::string msg = "");
  void to_fail_with(std::string failure_message, std::string msg = "");
  void to_match(std::regex regex, std::string msg = "");
  void to_match(std::string str, std::string msg = "");
  void to_partially_match(std::regex regex, std::string msg = "");
  void to_partially_match(std::string str, std::string msg = "");
  void to_satisfy(std::function<bool(A)> /*test*/, std::string msg = "");
  void to_start_with(std::string start, std::string msg = "");

  template <typename U>
  void to_contain(std::initializer_list<U> expected, std::string msg = "");

  template <typename E>
  void to_contain(E expected, std::string msg = "");

  template <typename U>
  void to_end_with(std::initializer_list<U> start, std::string msg = "");

  template <typename E>
  void to_equal(E expected, std::string msg = "");

  template <typename U>
  void to_start_with(std::initializer_list<U> start, std::string msg = "");

  void to_have_value(std::string msg = "");
#if __cpp_lib_expected
  void to_have_error(std::string msg = "");
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
void Expectation<A>::to(M matcher, std::string msg) {
  static_assert(std::is_base_of_v<Matchers::MatcherBase<A, typename M::expected_t>, M>,
                "Matcher is not a subclass of BaseMatcher.");
  // auto base_matcher = static_cast<Matchers::BaseMatcher<A,typename
  // M::expected_t>>(matcher);
  matcher.set_message(std::move(msg)).run();
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
void Expectation<A>::to_be_false(std::string msg) {
  static_assert(std::is_same_v<A, bool>,
                ".to_be_false() can only be used on booleans or functions that return booleans");
  to_equal(false, msg);
}

/**
 *
 *
 * @param msg
 *
 * @return
 */
template <typename A>
void Expectation<A>::to_be_falsy(std::string msg) {
  to_satisfy([](const A& t) { return !static_cast<bool>(t); }, msg);
}

/**
 * @brief Match using the Matchers::BeNullptr matcher.
 *
 * @param msg Optional message to give on failure.
 *
 * @return
 */
template <typename A>
void Expectation<A>::to_be_null(std::string msg) {
  Matchers::BeNullptr<A>(*this).set_message(std::move(msg)).run();
}

/**
 * @brief Match using the Matchers::Be matcher, testing for truthy-ness.
 *
 * @param msg Optional message to give on failure.
 *
 * @return
 */
template <typename A>
void Expectation<A>::to_be_true(std::string msg) {
  static_assert(std::is_same_v<A, bool>,
                ".to_be_true() can only be used on booleans or functions that return booleans");
  // return to_be([](A t) { return static_cast<bool>(t); }, msg);
  to_equal(true, msg);
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
void Expectation<A>::to_be_truthy(std::string msg) {
  to_satisfy([](const A& t) { return static_cast<bool>(t); }, msg);
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
void Expectation<A>::to_be_between(E min, E max, Matchers::RangeMode mode, std::string msg) {
  Matchers::BeBetween<A, E>(*this, min, max, mode).set_message(std::move(msg)).run();
}

template <typename A>
template <typename E>
void Expectation<A>::to_be_less_than(E rhs, std::string msg) {
  Matchers::BeLessThan<A, E>(*this, rhs).set_message(std::move(msg)).run();
}

template <typename A>
template <typename E>
void Expectation<A>::to_be_greater_than(E rhs, std::string msg) {
  Matchers::BeGreaterThan<A, E>(*this, rhs).set_message(std::move(msg)).run();
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
void Expectation<A>::to_contain(std::initializer_list<U> expected, std::string msg) {
  Matchers::Contain<A, std::vector<U>, U>(*this, expected).set_message(std::move(msg)).run();
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
void Expectation<A>::to_contain(E expected, std::string msg) {
  Matchers::Contain<A, E, E>(*this, expected).set_message(std::move(msg)).run();
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
void Expectation<A>::to_equal(E expected, std::string msg) {
  Matchers::Equal<A, E>(*this, expected).set_message(std::move(msg)).run();
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
  matcher.set_message(std::move(msg));
  return matcher;
}

template <typename A>
void Expectation<A>::to_fail(std::string msg) {
  static_assert(is_result_v<A>, ".to_fail() must be used on an expression that returns a Result.");
  Matchers::Fail<Result>(*this).set_message(std::move(msg)).run();
}

template <typename A>
void Expectation<A>::to_fail_with(std::string failure_message, std::string msg) {
  static_assert(is_result_v<A>, ".to_fail_with() must be used on an expression that returns a Result.");
  Matchers::FailWith<A>(*this, failure_message).set_message(std::move(msg)).run();
}

template <typename A>
void Expectation<A>::to_match(std::string str, std::string msg) {
  Matchers::Match<A>(*this, str).set_message(std::move(msg)).run();
}

template <typename A>
void Expectation<A>::to_match(std::regex regex, std::string msg) {
  Matchers::Match<A>(*this, regex).set_message(std::move(msg)).run();
}

template <typename A>
void Expectation<A>::to_partially_match(std::string str, std::string msg) {
  Matchers::MatchPartial<A>(*this, str).set_message(std::move(msg)).run();
}

template <typename A>
void Expectation<A>::to_partially_match(std::regex regex, std::string msg) {
  Matchers::MatchPartial<A>(*this, regex).set_message(std::move(msg)).run();
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
void Expectation<A>::to_satisfy(std::function<bool(A)> test, std::string msg) {
  Matchers::Satisfy<A>(*this, test).set_message(std::move(msg)).run();
}

template <typename A>
void Expectation<A>::to_start_with(std::string start, std::string msg) {
  Matchers::StartWith<std::string, std::string>(*this, start).set_message(std::move(msg)).run();
}

template <typename A>
template <typename U>
void Expectation<A>::to_start_with(std::initializer_list<U> start_sequence, std::string msg) {
  Matchers::StartWith<A, std::initializer_list<U>>(*this, start_sequence).set_message(std::move(msg)).run();
}

template <typename A>
void Expectation<A>::to_end_with(std::string ending, std::string msg) {
  Matchers::EndWith<std::string, std::string>(*this, ending).set_message(std::move(msg)).run();
}

template <typename A>
template <typename U>
void Expectation<A>::to_end_with(std::initializer_list<U> start_sequence, std::string msg) {
  Matchers::StartWith<A, std::initializer_list<U>>(*this, start_sequence).set_message(std::move(msg)).run();
}

template <typename A>
void Expectation<A>::to_have_value(std::string msg) {
  Matchers::HaveValue<A>(*this).set_message(std::move(msg)).run();
}

#if __cpp_lib_expected
template <typename A>
void Expectation<A>::to_have_error(std::string msg) {
  Matchers::HaveError<A>(*this).set_message(std::move(msg)).run();
}
#endif

template <typename A>
class ExpectationValue : public Expectation<A> {
  A value;

 public:
  /**
   * @brief Create an ExpectationValue using a value.
   *
   * @param value The target to test, an explicit value.
   *
   * @return The constructed ExpectationValue.
   */
  ExpectationValue(ItBase& it, A value, std::source_location location) : Expectation<A>(it, location), value(value) {}
  explicit ExpectationValue(A value, std::source_location location = std::source_location::current())
      : Expectation<A>(location), value(value) {}

  /**
   * @brief Create an Expectation using an initializer list.
   *
   * @param init_list The initializer list to match against.
   *
   * @return The constructed Expectation.
   */
  template <typename U>
  ExpectationValue(ItBase& it, std::initializer_list<U> init_list, std::source_location location)
      : Expectation<A>(it, location), value(std::vector<U>(init_list)) {}

  /** @brief Get the target of the expectation. */
  A& get_target() & override { return value; }

  ExpectationValue& not_() override {
    this->is_positive_ = not this->is_positive_;
    return *this;
  }

  ExpectationValue& ignore() override {
    this->ignore_ = true;
    return *this;
  }
};

template <Util::is_functional F>
class ExpectationFunc : public Expectation<decltype(std::declval<F>()())> {
  using block_ret_t = decltype(std::declval<F>()());
  F block;
  std::shared_ptr<block_ret_t> computed = nullptr;

 public:
  ExpectationFunc(ExpectationFunc<F> const& copy, std::source_location location)
      : Expectation<block_ret_t>(copy, location), block(copy.block) {}

  /**
   * @brief Create an ExpectationValue using a value.
   *
   * @param value The target to test, an explicit value.
   *
   * @return The constructed ExpectationValue.
   */
  ExpectationFunc(ItBase& it, F block, std::source_location location)
      : Expectation<block_ret_t>(it, location), block(block) {}

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
  block_ret_t& get_target() & override {
    if (computed == nullptr) {
      computed = std::make_shared<block_ret_t>(block());
    }
    return *computed;
  }

  // auto get_target() &  override -> decltype(std::declval<A>()()) & { return
  // Expectation<A>::get_target()(); }

  ExpectationFunc& not_() override {
    this->is_positive_ = !this->is_positive_;
    return *this;
  }

  ExpectationFunc& ignore() override {
    this->ignore_ = true;
    return *this;
  }

  Expectation<decltype(block())>& casted() { return static_cast<decltype(block())>(*this); }

  template <typename Ex = std::exception>
  void to_throw(std::string msg = "");
};

template <Util::is_functional F>
template <typename Ex>
void ExpectationFunc<F>::to_throw(std::string msg) {
  Matchers::Throw<decltype(this->block.operator()()), Ex>(*this).set_message(std::move(msg)).run();
}

}  // namespace CppSpec

/**
 * @file
 * @brief Defines the Expectations::Expectation class and associated functions
 */
#ifndef EXPECTATION_H
#define EXPECTATION_H
#include "let.hpp"
#include "matchers/be.hpp"
#include "matchers/be_between.hpp"
#include "matchers/be_within.hpp"
#include "matchers/include.hpp"
#include "matchers/equal.hpp"

namespace Expectations {

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
  A target;
  std::function<A(void)> block;
  bool has_block = false;   // Is the target a lambda?
  bool is_positive = true;  // Have we been negated?

  template <typename E>
  bool setup_and_run(Matchers::BaseMatcher<A, E> &matcher, std::string msg);

 public:
  /**
   * @brief Create an Expectation using a value.
   *
   * @param value The target to test, an explicit value.
   *
   * @return The constructed Expectation.
   */
  Expectation(A &value) : target(value) {}

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
  Expectation(std::function<A(void)> block) : block(block), has_block(true) {}

  /**
   * @brief Create an Expectation using an initializer list.
   *
   * @param init_list The initializer list to match against.
   *
   * @return The constructed Expectation.
   */
  template <typename U>
  Expectation(std::initializer_list<U> init_list)
      : target(std::vector<U>(init_list)) {}

  /** @brief Get the target of the expectation. */
  A &get_target() { return target; }

  /** @brief Get whether the expectation is normal or negated. */
  bool get_sign() { return is_positive; }

  Expectation &not_();

  bool to_be(std::function<bool(A)>, std::string msg = "");
  bool to_be_null(std::string msg = "");
  bool to_be_true(std::string msg = "");
  bool to_be_false(std::string msg = "");

  template <typename U>
  bool to_include(std::initializer_list<U> expected, std::string msg = "");

  template <typename E>
  bool to_include(E expected, std::string msg = "");

  template <typename E>
  bool to_be_between(E min, E max, Matchers::RangeMode mode,
                     std::string msg = "");

  template <typename E>
  bool to_be_between(E min, E max, std::string msg = "");

  template <typename E>
  bool to_equal(E expected, std::string msg = "");

  template <typename E>
  bool to_be_within(E expected, std::string msg = "");
};

/**
 * @brief Invert the current matcher.
 */
template <typename A>
Expectation<A> &Expectation<A>::not_() {
  is_positive = not is_positive;
  return *this;
}

// It's easier to set the parents of the matchers
// in these functions instead of doing it in their respective constructors.
// Also, it follows the pattern we've been using so far.
// The parents of the matchers are not Expectations, but instead the
// "it" that contains them (i.e. self->parent), as the Matchers are what truly
// get executed in the "it"s

template <typename A>
template <typename E>
bool Expectation<A>::setup_and_run(Matchers::BaseMatcher<A, E> &matcher,
                                   std::string msg) {
  matcher.set_parent(this->get_parent());
  return matcher(msg);
}

/** 
 * @brief Match using the Matchers::Be matcher.
 *
 * @param test The function to use to test the output of the
 *             expectation expression.
 * @param msg Optional message to give on failure.
 *
 * @return Whether the expectation succeeds or fails.
 */ template <typename A>
bool Expectation<A>::to_be(std::function<bool(A)> test, std::string msg) {
  Matchers::Be<A> matcher(*this, test);
  return setup_and_run(matcher, msg);
}

/**
 * @brief Match using the Matchers::BeNullptr matcher.
 *
 * @param msg Optional message to give on failure.
 *
 * @return
 */
template <typename A>
bool Expectation<A>::to_be_null(std::string msg) {
  Matchers::BeNullptr<A> matcher(*this);
  return setup_and_run(matcher, msg);
}

/**
 * @brief Match using the Matchers::Be matcher, testing for truthy-ness.
 *
 * @param msg Optional message to give on failure.
 *
 * @return
 */
template <typename A>
bool Expectation<A>::to_be_true(std::string msg) {
  Matchers::Be<A> matcher(*this, [](A t) { return static_cast<bool>(t); });
  return setup_and_run(matcher, msg);
}

/**
 * @brief Match using the Matchers::Be matcher, testing for falsy-ness.
 *
 * @param msg Optional message to give on failure.
 *
 * @return
 */
template <typename A>
bool Expectation<A>::to_be_false(std::string msg) {
  Matchers::Be<A> matcher(*this, [](A t) { return not static_cast<bool>(t); });
  return setup_and_run(matcher, msg);
}

/**
 * @brief Match using the Matchers::BeBetween matcher.
 *
 * @param min
 * @param max
 * @param mode
 * @param msg Optional message to give on failure.
 *
 * @return
 */
template <typename A>
template <typename E>
bool Expectation<A>::to_be_between(E min, E max, Matchers::RangeMode mode,
                                   std::string msg) {
  Matchers::BeBetween<A, E> matcher(*this, min, max, mode);
  return setup_and_run(matcher, msg);
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
bool Expectation<A>::to_be_between(E min, E max, std::string msg) {
  return this->to_be_between(min, max, Matchers::RangeMode::inclusive, msg);
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
bool Expectation<A>::to_include(std::initializer_list<U> expected,
                                std::string msg) {
  Matchers::Include<A, std::vector<U>, U> matcher(*this, expected);
  return setup_and_run(matcher, msg);
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
bool Expectation<A>::to_include(E expected, std::string msg) {
  Matchers::Include<A, E, E> matcher(*this, expected);
  return setup_and_run(matcher, msg);
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
bool Expectation<A>::to_equal(E expected, std::string msg) {
  Matchers::Equal<A, E> matcher(*this, expected);
  return setup_and_run(matcher, msg);
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
bool Expectation<A>::to_be_within(E expected, std::string msg) {
  Matchers::BeWithin<A, E> matcher(*this, expected);
  return setup_and_run(matcher, msg);
}
}

#endif /* EXPECTATION_H */

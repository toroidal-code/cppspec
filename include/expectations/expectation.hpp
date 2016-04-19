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
  A target;  // An expectation literally contains the object from `expect(obj)`
  std::function<A(void)> block;
  bool has_block = false;   // Is the target a lambda?
  bool is_positive = true;  // Have we been negated?

 public:
  Expectation(Expectation const &copy)
      : Child(copy.get_parent()),
        target(copy.target),
        block(copy.block),
        has_block(copy.has_block),
        is_positive(copy.is_positive){};

  /**
   * @brief Create an Expectation using a value.
   *
   * @param value The target to test, an explicit value.
   *
   * @return The constructed Expectation.
   */
  Expectation(ItBase &it, A value) : Child(it), target(value) {}

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
  Expectation(ItBase &it, std::function<A(void)> block)
      : Child(it), block(block), has_block(true) {}

  /**
   * @brief Create an Expectation using an initializer list.
   *
   * @param init_list The initializer list to match against.
   *
   * @return The constructed Expectation.
   */
  template <typename U>
  Expectation(ItBase &it, std::initializer_list<U> init_list)
      : Child(it), target(std::vector<U>(init_list)) {}

  /** @brief Get the target of the expectation. */
  A &get_target() { return target; }

  /** @brief Get whether the expectation is normal or negated. */
  bool get_sign() { return is_positive; }

  Expectation &not_();

  template <class M>
  bool to(M matcher, std::string msg = "");

  bool to_be(std::function<bool(A)>, std::string msg = "");
  bool to_be_null(std::string msg = "");
  bool to_be_true(std::string msg = "");
  bool to_be_false(std::string msg = "");

  template <typename U>
  bool to_include(std::initializer_list<U> expected, std::string msg = "");

  template <typename E>
  bool to_include(E expected, std::string msg = "");

  template <typename E>
  Matchers::BeBetween<A, E> to_be_between(E min, E max, std::string msg = "");

  template <typename E>
  bool to_equal(E expected, std::string msg = "");

  template <typename E>
  Matchers::BeWithin<A, E> to_be_within(E expected, std::string msg = "");
};

/**
 * @brief Invert the current matcher.
 */
template <typename A>
Expectation<A> &Expectation<A>::not_() {
  this->is_positive = not this->is_positive;
  return *this;
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
  return Matchers::Be<A>(*this, test).set_message(msg).run();
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
  return Matchers::BeNullptr<A>(*this).set_message(msg).run();
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
  return Matchers::Be<A>(*this, [](A t) {
           return static_cast<bool>(t);
         }).set_message(msg).run();
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
  return Matchers::Be<A>(*this, [](A t) {
           return not static_cast<bool>(t);
         }).set_message(msg).run();
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
Matchers::BeBetween<A, E> Expectation<A>::to_be_between(E min, E max,
                                                        std::string msg) {
  Matchers::BeBetween<A, E> matcher(*this, min, max);
  matcher.set_message(msg);
  return matcher;
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
  return Matchers::Include<A, std::vector<U>, U>(*this, expected)
      .set_message(msg)
      .run();
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
  return Matchers::Include<A, E, E>(*this, expected).set_message(msg).run();
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
  return Matchers::Equal<A, E>(*this, expected).set_message(msg).run();
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
Matchers::BeWithin<A, E> Expectation<A>::to_be_within(E expected,
                                                      std::string msg) {
  Matchers::BeWithin<A, E> matcher(*this, expected);
  matcher.set_message(msg);
  return matcher;
}

template <typename A>
template <class M>
bool Expectation<A>::to(M matcher, std::string msg) {
  static_assert(
      std::is_base_of<Matchers::BaseMatcher<A, typename M::expected_t>,
                      M>::value,
      "Matcher is not a subclass of BaseMatcher.");
  // auto base_matcher = static_cast<Matchers::BaseMatcher<A,typename
  // M::expected_t>>(matcher);
  return matcher.set_message(msg).run();
}
}

#endif /* EXPECTATION_H */

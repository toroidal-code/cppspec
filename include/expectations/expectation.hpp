#ifndef EXPECTATION_H
#define EXPECTATION_H
#include <functional>
#include <vector>
#include "runnable.hpp"
#include "matchers/basematcher.hpp"
#include "matchers/be_between.hpp"
#include "matchers/include.hpp"
#include "matchers/equal.hpp"
#include "matchers/be_within.hpp"

namespace Expectations {

/**
 * @brief Wraps the target of an expectation
 * @code
 *   expect(something)
 *   expect([] -> auto { return something })
 *
 *   // used with `to` matchers
 *   expect(actual).to_equal(target)
 *
 *   // used with `not` and matchers
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
  bool has_block = false;
  bool is_positive = true;

  template <typename E>
  bool setup_and_run(Matchers::BaseMatcher<A, E> &matcher, std::string msg);

 public:
  Expectation(A &value) : target(value) {}

  Expectation(std::function<A(void)> block) : block(block), has_block(true) {}

  template <typename U>
  Expectation(std::initializer_list<U> init_list)
      : target(std::vector<U>(init_list)) {}

  A &get_target();
  bool get_sign();

  Expectation &not_();
  Expectation &to();

  template <typename U>
  bool to_include(std::initializer_list<U> expected, std::string msg = "");

  template <typename U>
  bool to_include(U expected, std::string msg = "");

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

template <typename A>
A &Expectation<A>::get_target() {
  return target;
}

template <typename A>
bool Expectation<A>::get_sign() {
  return is_positive;
}

/**
 * @brief Inverts the current matcher
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

template <typename A>
template <typename E>
bool Expectation<A>::to_be_between(E min, E max, Matchers::RangeMode mode,
                                   std::string msg) {
  Matchers::BeBetween<A, E> matcher(*this, min, max, mode);
  return setup_and_run(matcher, msg);
}

template <typename A>
template <typename E>
bool Expectation<A>::to_be_between(E min, E max, std::string msg) {
  return this->to_be_between(min, max, Matchers::RangeMode::inclusive, msg);
}

template <typename A>
template <typename U>
bool Expectation<A>::to_include(std::initializer_list<U> expected,
                                std::string msg) {
  Matchers::Include<A, std::vector<U>, U> matcher(*this, expected);
  return setup_and_run(matcher, msg);
}

template <typename A>
template <typename U>
bool Expectation<A>::to_include(U expected, std::string msg) {
  Matchers::Include<A, U, U> matcher(*this, expected);
  return setup_and_run(matcher, msg);
}

template <typename A>
template <typename E>
bool Expectation<A>::to_equal(E expected, std::string msg) {
  Matchers::Equal<A, E> matcher(*this, expected);
  return setup_and_run(matcher, msg);
}

template <typename A>
template <typename E>
bool Expectation<A>::to_be_within(E expected, std::string msg) {
  Matchers::BeWithin<A, E> matcher(*this, expected);
  return setup_and_run(matcher, msg);
}
}

#endif /* EXPECTATION_H */

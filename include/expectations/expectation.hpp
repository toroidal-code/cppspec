#ifndef EXPECTATION_H
#define EXPECTATION_H
#include <functional>
#include <vector>
#include "runnable.hpp"
#include "matchers/be_between.hpp"
#include "matchers/include.hpp"
#include "matchers/equal.hpp"
#include "matchers/be_within.hpp"

namespace Expectations {

/**
 * Wraps the target of an expectation
 * @example
 *   expect(something)
 *   expect([] -> auto { return something })
 *
 *   # used with `to`
 *   expect(actual).to_equal(target)()
 *
 *   # used with `not_to`
 *   expect(actual).not_.to_equal(target)()
 *
 * Static typing forces us to either give the type of the expected result
 * to the Matcher object, or chain in a builder-like fashion.
 * Thus, Expectations
 * need to be a builder pattern with an explicit `invoke` directive.
 * I chose the call operator `()`, because it seemed the most logical.
 */
template <class A>
class Expectation : public Child {
  A target;
  std::function<A(void)> block;
  bool has_block = false;
  bool is_positive = true;

 public:
  Expectation(A &value) : target(value) {}

  Expectation(std::function<A(void)> block) : block(block), has_block(true) {}

  template <typename U>
  Expectation(std::initializer_list<U> init_list)
      : target(std::vector<U>(init_list)) {}

  A &get_target();
  bool get_sign();

  Expectation &not_();
  Expectation &to_be();

  template <typename U>
  Matchers::Include<A, std::vector<U>, U> to_include(
      std::initializer_list<U> expected);

  template <typename U>
  Matchers::Include<A, U, U> to_include(U expected);

  template <typename E>
  Matchers::BeBetween<A, E> to_be_between(E min, E max);

  template <typename E>
  Matchers::Equal<A, E> to_equal(E expected);

  template <typename E>
  Matchers::BeWithin<A, E> to_be_within(E expected);
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
Matchers::BeBetween<A, E> Expectation<A>::to_be_between(E min, E max) {
  auto matcher = new Matchers::BeBetween<A, E>(*this, min, max);
  matcher->set_parent(this->get_parent());
  return *matcher;
}

template <typename A>
template <typename U>
Matchers::Include<A, std::vector<U>, U> Expectation<A>::to_include(
    std::initializer_list<U> expected) {
  auto matcher = new Matchers::Include<A, std::vector<U>, U>(*this, expected);
  matcher->set_parent(this->get_parent());
  return *matcher;
}

template <typename A>
template <typename U>
Matchers::Include<A, U, U> Expectation<A>::to_include(U expected) {
  auto matcher = new Matchers::Include<A, U, U>(*this, expected);
  matcher->set_parent(this->get_parent());
  return *matcher;
}

template <typename A>
template <typename E>
Matchers::Equal<A, E> Expectation<A>::to_equal(E expected) {
  auto matcher = new Matchers::Equal<A, E>(*this, expected);
  matcher->set_parent(this->get_parent());
  return *matcher;
}

template <typename A>
template <typename E>
Matchers::BeWithin<A, E> Expectation<A>::to_be_within(E expected) {
  auto matcher = new Matchers::BeWithin<A, E>(*this, expected);
  matcher->set_parent(this->get_parent());
  return *matcher;
}
}
#endif /* EXPECTATION_H */

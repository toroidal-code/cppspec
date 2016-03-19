#ifndef INCLUDE_H
#define INCLUDE_H
#include <string>
#include "../util.hpp"

#include "basematcher.hpp"

namespace Matchers {

/**
 * The abstract base class for the Include matcher.
 * See template specializations below.
 */
template <typename A, typename E, typename U>
class IncludeBase : public BaseMatcher<A, E> {
 public:
  virtual bool matches(A actual) = 0;
  virtual bool does_not_match(A actual) = 0;
  virtual std::string description();
  virtual std::string failure_message();
  virtual std::string failure_message_when_negated();
  virtual bool diffable() { return true; }

 protected:
  bool actual_collection_includes(U expected_item);
  IncludeBase(Expectations::Expectation<A> expectation,
              std::initializer_list<U> expected)
      : BaseMatcher<A, std::vector<U>>(expectation, std::vector<U>(expected)){};
  IncludeBase(Expectations::Expectation<A> expectation, U expected)
      : BaseMatcher<A, U>(expectation, expected){};
};

template <typename A, typename E, typename U>
std::string IncludeBase<A, E, U>::description() {
  // std::vector<E> described_items;
  return Pretty::improve_hash_formatting("include" +
                                         Pretty::to_sentance(this->expected));
}

template <typename A, typename E, typename U>
std::string IncludeBase<A, E, U>::failure_message() {
  return Pretty::improve_hash_formatting(BaseMatcher<A, E>::failure_message());
}

template <typename A, typename E, typename U>
std::string IncludeBase<A, E, U>::failure_message_when_negated() {
  return Pretty::improve_hash_formatting(
      BaseMatcher<A, E>::failure_message_when_negated());
}

template <typename A, typename E, typename U>
bool IncludeBase<A, E, U>::actual_collection_includes(U expected_item) {
  auto last = *(this->actual.begin());
  static_assert(
      Util::verbose_assert<std::is_same<decltype(last), U>>::value,
      "Expected item is not the same type as what is inside container.");
  return std::find(this->actual.begin(), this->actual.end(), expected_item) !=
         this->actual.end();
}

/**
 * The template specialization of Include that permits matching
 * against a list of elements (i.e. expected({1,2,3,4}).to_include({1,4}) )
 */
template <typename A, typename E, typename U>
class Include : public IncludeBase<A, E, U> {
  enum class Predicate { any, all, none };

 public:
  virtual bool matches(A actual);
  virtual bool does_not_match(A actual);
  Include(Expectations::Expectation<A> expectation,
          std::initializer_list<U> expected)
      : IncludeBase<A, E, U>(expectation, expected){};

 protected:
  bool perform_match(Predicate predicate, Predicate hash_subset_predicate);
};

template <typename A, typename E, typename U>
bool Include<A, E, U>::matches(A actual) {
  this->actual = actual;
  return perform_match(Predicate::all, Predicate::all);
}

template <typename A, typename E, typename U>
bool Include<A, E, U>::does_not_match(A actual) {
  this->actual = actual;
  return perform_match(Predicate::none, Predicate::any);
}

// TODO: support std::map<E,_>
template <typename A, typename E, typename U>
bool Include<A, E, U>::perform_match(Predicate predicate,
                                     Predicate /*hash_subset_predicate*/) {
  bool retval = true;  // start off true
  for (U expected_item : this->expected) {
    retval = retval && this->actual_collection_includes(expected_item);

    // Based on our main predicate
    switch (predicate) {
      case Predicate::all:
        if (retval) {
          continue;  // continue the loop
        } else {
          goto exit_loop;  // *GASP* code jumping!
        }
      case Predicate::none:
        if (retval) {
          goto exit_loop;
        } else {
          continue;
        }
      case Predicate::any:
        retval = false;
        goto exit_loop;
    }
  }
exit_loop:;  // Wheeeeeee
  return retval;
}

/**
 * The template specialization of Include that permits matching
 * of a single element (i.e. expected({1,2,3,4}).to_include(4) )
 */
template <typename A, typename U>
class Include<A, U, U> : public IncludeBase<A, U, U> {
 public:
  Include(Expectations::Expectation<A> expectation, U expected)
      : IncludeBase<A, U, U>(expectation, expected){};

  virtual bool matches(A actual);
  virtual bool does_not_match(A actual);
};

template <typename A, typename U>
bool Include<A, U, U>::matches(A actual) {
  this->actual = actual;
  return this->actual_collection_includes(this->expected);
}

template <typename A, typename U>
bool Include<A, U, U>::does_not_match(A actual) {
  return !matches(actual);
}
}

#endif /* INCLUDE_H */

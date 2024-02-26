/** @file */
#pragma once
#include "matcher_base.hpp"

namespace CppSpec::Matchers {

/**
 * The abstract base class for the Include matcher.
 * See template specializations below.
 */
template <typename A, typename E, typename U>
class ContainBase : public MatcherBase<A, E> {
  A actual_;

 public:
  std::string description() override;
  std::string failure_message() override;
  std::string failure_message_when_negated() override;
  virtual bool diffable() { return true; }

  ContainBase(Expectation<A> &expectation, std::initializer_list<U> expected)
      : MatcherBase<A, std::vector<U>>(expectation, std::vector<U>(expected)), actual_(this->actual()){};
  ContainBase(Expectation<A> &expectation, U expected)
      : MatcherBase<A, U>(expectation, expected), actual_(this->actual()){};

 protected:
  bool actual_collection_includes(U expected_item);
};

template <typename A, typename E, typename U>
std::string ContainBase<A, E, U>::description() {
  // std::vector<E> described_items;
  return Pretty::improve_hash_formatting("contain" + Pretty::to_sentence(this->expected()));
}

template <typename A, typename E, typename U>
std::string ContainBase<A, E, U>::failure_message() {
  return Pretty::improve_hash_formatting(MatcherBase<A, E>::failure_message());
}

template <typename A, typename E, typename U>
std::string ContainBase<A, E, U>::failure_message_when_negated() {
  return Pretty::improve_hash_formatting(MatcherBase<A, E>::failure_message_when_negated());
}

template <typename A, typename E, typename U>
bool ContainBase<A, E, U>::actual_collection_includes(U expected_item) {
  auto actual = this->actual();
  auto last = *(actual.begin());
  static_assert(Util::verbose_assert<std::is_same_v<decltype(last), U>>::value,
                "Expected item is not the same type as what is inside container.");
  return std::find(actual.begin(), actual.end(), expected_item) != actual.end();
}

/**
 * The template specialization of Include that permits matching
 * against a list of elements (i.e. expected({1,2,3,4}).to_include({1,4}) )
 */
template <typename A, typename E, typename U>
class Contain : public ContainBase<A, E, U> {
  enum class Predicate { any, all, none };

 public:
  bool match() override;
  bool negated_match() override;
  using ContainBase<A, E, U>::ContainBase;

 protected:
  bool perform_match(Predicate predicate, Predicate hash_subset_predicate);
};

template <typename A, typename E, typename U>
bool Contain<A, E, U>::match() {
  return perform_match(Predicate::all, Predicate::all);
}

template <typename A, typename E, typename U>
bool Contain<A, E, U>::negated_match() {
  return perform_match(Predicate::none, Predicate::any);
}

// TODO: support std::map<E,_>
template <typename A, typename E, typename U>
bool Contain<A, E, U>::perform_match(Predicate predicate, Predicate /*hash_subset_predicate*/) {
  bool retval = true;  // start off true
  for (U expected_item : this->expected()) {
    retval = retval && this->actual_collection_includes(expected_item);

    // Based on our main predicate
    switch (predicate) {
      case Predicate::all:
        if (retval) {    // if the collection includes the item,
          continue;      // continue the loop
        } else {         // otherwise
          return false;  // immediately return false
        }
      case Predicate::none:
        if (retval) {    // if the collection includes the item
          return false;  // immediately return false
        } else {         // otherwise
          continue;      // continue the loop
        }
      case Predicate::any:
        if (retval) {   // if the collection includes the item
          return true;  // immediately return true
        } else {        // otherwise
          continue;     // continue the loop
        }
    }
  }
  return true;
}

/**
 * The template specialization of Include that permits matching
 * of a single element (i.e. expected({1,2,3,4}).to_include(4) )
 */
template <typename A, typename U>
class Contain<A, U, U> : public ContainBase<A, U, U> {
 public:
  Contain(Expectation<A> &expectation, U expected) : ContainBase<A, U, U>(expectation, expected){};

  bool match() override;
};

template <typename A, typename U>
bool Contain<A, U, U>::match() {
  return this->actual_collection_includes(this->expected());
}

}  // namespace CppSpec::Matchers


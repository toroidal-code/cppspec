/** @file */
#pragma once

#include <ranges>
#include <string>

#include "matchers/matcher_base.hpp"

namespace CppSpec::Matchers {

// Text is compared through string_view_of(), whatever form it is in; any other pair of ranges element by element.
template <typename A, typename E>
  requires(Util::is_string_like<A> && Util::is_string_like<E>) || (std::ranges::range<A> && std::ranges::range<E>)
class StartWith : public MatcherBase<A, E> {
  static constexpr bool compares_text = Util::is_string_like<A> && Util::is_string_like<E>;

 public:
  StartWith(Expectation<A>& expectation, E start) : MatcherBase<A, E>(expectation, start) {}

  std::string verb() override { return "start with"; }
  std::string failure_message() override {
    return MatcherBase<A, E>::failure_message() + no_text_to_search(this->actual());
  }
  std::string failure_message_when_negated() override {
    return MatcherBase<A, E>::failure_message_when_negated() + no_text_to_search(this->actual());
  }

  bool match() override {
    if constexpr (compares_text) {
      const auto actual = string_view_of(this->actual());
      const auto expected = string_view_of(this->expected());
      return actual && expected && actual->starts_with(*expected);
    } else {
      A& actual = this->actual();
      E& expected = this->expected();
      // std::equal reads as many elements of the actual range as the expected one has, so a shorter actual range
      // fails before it is read.
      return std::ranges::distance(expected) <= std::ranges::distance(actual) &&
             std::equal(expected.begin(), expected.end(), actual.begin());
    }
  }

  // A null C string is no text, so it fails negated as well as positive.
  bool negated_match() override {
    if constexpr (compares_text) {
      const auto actual = string_view_of(this->actual());
      const auto expected = string_view_of(this->expected());
      return actual && expected && !actual->starts_with(*expected);
    } else {
      return !match();
    }
  }
};

}  // namespace CppSpec::Matchers

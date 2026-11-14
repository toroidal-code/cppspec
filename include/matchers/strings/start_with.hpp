/** @file */
#pragma once

#include <algorithm>
#include <ranges>
#include <string>

#include "matchers/matcher_base.hpp"

namespace CppSpec::Matchers {

/**
 * @brief The `start with` matcher
 *
 * Text is compared through string_view_of(), whatever form it is in. Any other pair of ranges is compared element
 * by element; a range of characters can be compared with text too. A null C string, as either value, is no text, so
 * the matcher fails it negated as well as positive.
 *
 * @tparam A the type of the actual value
 * @tparam E the type of the expected value
 */
template <typename A, typename E>
  requires Util::compares_start_and_end<A, E>
class StartWith : public MatcherBase<A, E> {
  static constexpr bool compares_text = Util::is_string_like<A> && Util::is_string_like<E>;

  // Whether both values hold something to compare: neither is a null C string.
  bool has_values() {
    if constexpr (Util::is_string_like<A>) {
      if (!string_view_of(this->actual())) {
        return false;
      }
    }
    if constexpr (Util::is_string_like<E>) {
      return string_view_of(this->expected()).has_value();
    } else {
      return true;
    }
  }

  // The actual value against the expected one, both known to be there.
  bool compare() {
    if constexpr (compares_text) {
      return string_view_of(this->actual())->starts_with(*string_view_of(this->expected()));
    } else {
      A& actual = this->actual();
      auto compare_with = [&actual](const auto& expected) {
        // std::equal reads as many elements of the actual range as the expected one has, so a shorter actual range
        // fails before it is read.
        return std::ranges::distance(expected) <= std::ranges::distance(actual) &&
               std::equal(expected.begin(), expected.end(), actual.begin());
      };
      if constexpr (Util::is_string_like<E>) {
        return compare_with(*string_view_of(this->expected()));
      } else {
        return compare_with(this->expected());
      }
    }
  }

 public:
  StartWith(Expectation<A>& expectation, E start) : MatcherBase<A, E>(expectation, start) {}

  std::string verb() override { return "start with"; }
  std::string failure_message() override {
    return MatcherBase<A, E>::failure_message() + no_text_to_search(this->actual()) +
           no_text_to_look_for(this->expected());
  }
  std::string failure_message_when_negated() override {
    return MatcherBase<A, E>::failure_message_when_negated() + no_text_to_search(this->actual()) +
           no_text_to_look_for(this->expected());
  }

  bool match() override { return has_values() && compare(); }
  bool negated_match() override { return has_values() && !compare(); }
};

}  // namespace CppSpec::Matchers

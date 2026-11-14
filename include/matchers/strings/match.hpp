/** @file */
#pragma once
#include <regex>
#include <string>

#include "matchers/matcher_base.hpp"

namespace CppSpec::Matchers {

// A std::regex keeps no copy of its source, so a pattern given as text is named by that text; one given as a
// std::regex is named only as a regex.
inline std::string describe_pattern(const std::string& pattern) {
  return pattern.empty() ? std::string{"a regex"} : "\"" + pattern + "\"";
}

/**
 * @brief Compile a pattern given as text, read through string_view_of(); a null C string compiles to an empty regex,
 *        which a matcher never uses, as it has no pattern to look for
 */
template <Util::is_string_like T>
[[nodiscard]] std::regex regex_of(const T& pattern) {
  const auto text = string_view_of(pattern);
  return text ? std::regex(text->begin(), text->end()) : std::regex();
}

/**
 * @brief Name a pattern given as text: its quoted text, or `nullptr` for a null C string
 */
template <Util::is_string_like T>
[[nodiscard]] std::string pattern_name_of(const T& pattern) {
  const auto text = string_view_of(pattern);
  return text ? describe_pattern(std::string(*text)) : std::string("nullptr");
}

/**
 * @brief The `match` matcher: the whole of the actual text matches a regular expression
 *
 * The actual text is read through string_view_of(). A null C string, as the actual text or as the pattern, is no
 * text, so the matcher fails it negated as well as positive.
 *
 * @tparam A the type of the actual value
 */
template <Util::is_string_like A>
class Match : public MatcherBase<A, std::regex> {
  std::string pattern_name_ = describe_pattern("");
  bool has_pattern_ = true;

  // Whether there is text to match and a pattern to match it with.
  bool has_values() { return has_pattern_ && string_view_of(this->actual()).has_value(); }

  bool matches() {
    const auto actual = string_view_of(this->actual());
    return std::regex_match(actual->begin(), actual->end(), this->expected());
  }

 public:
  template <Util::is_string_like T>
  explicit Match(Expectation<A>& expectation, const T& expected)
      : MatcherBase<A, std::regex>(expectation, regex_of(expected)),
        pattern_name_(pattern_name_of(expected)),
        has_pattern_(string_view_of(expected).has_value()) {}

  explicit Match(Expectation<A>& expectation, std::regex expected)
      : MatcherBase<A, std::regex>(expectation, expected) {}

  std::string verb() override { return "match"; }
  std::string description() override { return verb() + " " + pattern_name_; }
  std::string failure_message() override {
    return MatcherBase<A, std::regex>::failure_message() + no_text_to_search(this->actual()) + no_pattern_note();
  }
  std::string failure_message_when_negated() override {
    return MatcherBase<A, std::regex>::failure_message_when_negated() + no_text_to_search(this->actual()) +
           no_pattern_note();
  }

  bool match() override { return has_values() && matches(); }
  bool negated_match() override { return has_values() && !matches(); }

 private:
  std::string no_pattern_note() { return has_pattern_ ? std::string() : std::string(nothing_to_look_for); }
};

/**
 * @brief The `partially match` matcher: a regular expression matches somewhere in the actual text
 *
 * The actual text is read through string_view_of(). A null C string, as the actual text or as the pattern, is no
 * text, so the matcher fails it negated as well as positive.
 *
 * @tparam A the type of the actual value
 */
template <Util::is_string_like A>
class MatchPartial : public MatcherBase<A, std::regex> {
  std::string pattern_name_ = describe_pattern("");
  bool has_pattern_ = true;

  // Whether there is text to search and a pattern to search it for.
  bool has_values() { return has_pattern_ && string_view_of(this->actual()).has_value(); }

  // A partial match is a match anywhere in the string: a search, not a match of the whole.
  bool matches() {
    const auto actual = string_view_of(this->actual());
    return std::regex_search(actual->begin(), actual->end(), this->expected());
  }

 public:
  template <Util::is_string_like T>
  explicit MatchPartial(Expectation<A>& expectation, const T& expected)
      : MatcherBase<A, std::regex>(expectation, regex_of(expected)),
        pattern_name_(pattern_name_of(expected)),
        has_pattern_(string_view_of(expected).has_value()) {}

  explicit MatchPartial(Expectation<A>& expectation, std::regex expected)
      : MatcherBase<A, std::regex>(expectation, expected) {}

  std::string description() override { return "partially match " + pattern_name_; }
  std::string failure_message() override {
    return MatcherBase<A, std::regex>::failure_message() + no_text_to_search(this->actual()) + no_pattern_note();
  }
  std::string failure_message_when_negated() override {
    return MatcherBase<A, std::regex>::failure_message_when_negated() + no_text_to_search(this->actual()) +
           no_pattern_note();
  }

  bool match() override { return has_values() && matches(); }
  bool negated_match() override { return has_values() && !matches(); }

 private:
  std::string no_pattern_note() { return has_pattern_ ? std::string() : std::string(nothing_to_look_for); }
};

}  // namespace CppSpec::Matchers

/** @file */
#pragma once
#include <string>

#include "matchers/matcher_base.hpp"
#include "util.hpp"

namespace CppSpec::Matchers {

/**
 * @brief Matches a value above the expected one: `actual > expected`, compared by value when both are integers
 *        (`-1` is not above `0u`). Under `not_()` it matches `actual <= expected`, so NaN, which neither holds for,
 *        fails both ways.
 */
template <typename A, typename E>
class BeGreaterThan : public MatcherBase<A, E> {
 public:
  BeGreaterThan(Expectation<A>& expectation, E expected) : MatcherBase<A, E>(expectation, expected) {}
  bool match() override { return Util::greater(this->actual(), this->expected()); }
  bool negated_match() override { return Util::less_equal(this->actual(), this->expected()); }
  std::string verb() override { return "be greater than"; }
};

}  // namespace CppSpec::Matchers

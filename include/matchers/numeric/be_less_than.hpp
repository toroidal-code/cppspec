/** @file */
#pragma once
#include <string>

#include "matchers/matcher_base.hpp"
#include "util.hpp"

namespace CppSpec::Matchers {

/**
 * @brief Matches a value below the expected one: `actual < expected`, compared by value when both are integers
 *        (`-1` is below `0u`). Under `not_()` it matches `actual >= expected`, so NaN, which neither holds for, fails
 *        both ways.
 */
template <typename A, typename E>
class BeLessThan : public MatcherBase<A, E> {
 public:
  BeLessThan(Expectation<A>& expectation, E expected) : MatcherBase<A, E>(expectation, expected) {}
  bool match() override { return Util::less(this->actual(), this->expected()); }
  bool negated_match() override { return Util::greater_equal(this->actual(), this->expected()); }
  std::string verb() override { return "be less than"; }
};

}  // namespace CppSpec::Matchers

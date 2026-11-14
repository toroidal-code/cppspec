/** @file */
#pragma once

#include "matchers/matcher_base.hpp"
#include "util.hpp"

namespace CppSpec::Matchers {

enum class RangeMode { exclusive, inclusive };

/**
 * @brief Matches a value between two bounds, inclusive or exclusive, compared by value when both sides are integers
 *        (`-1` is not between `0u` and `10u`). Under `not_()` it matches a value below the lower bound or above the
 *        upper one, so NaN, which is neither, fails both ways.
 */
template <typename A, typename E>
class BeBetween : public MatcherBase<A, E> {
  E min;
  E max;
  RangeMode mode;
  enum class LtOp { lt, lt_eq } lt_op;
  enum class GtOp { gt, gt_eq } gt_op;

 public:
  //  BeBetween(Expectation<A> &expectation, E min, E max)
  //      : BaseMatcher<A, E>(expectation), min(min), max(max) {}

  BeBetween(Expectation<A>& expectation, E min, E max, RangeMode mode = RangeMode::inclusive)
      : MatcherBase<A, E>(expectation), min(min), max(max), mode(mode) {
    switch (mode) {
      case RangeMode::inclusive:
        lt_op = LtOp::lt_eq;
        gt_op = GtOp::gt_eq;
        break;
      case RangeMode::exclusive:
        lt_op = LtOp::lt;
        gt_op = GtOp::gt;
        break;
    }
  }

  bool match() override;
  bool negated_match() override;
  std::string verb() override { return "be between"; }
  std::string description() override;
};

template <typename A, typename E>
bool BeBetween<A, E>::match() {
  const auto& actual = this->actual();
  const bool above_min = gt_op == GtOp::gt ? Util::greater(actual, min) : Util::greater_equal(actual, min);
  const bool below_max = lt_op == LtOp::lt ? Util::less(actual, max) : Util::less_equal(actual, max);
  return above_min && below_max;
}

template <typename A, typename E>
bool BeBetween<A, E>::negated_match() {
  const auto& actual = this->actual();
  const bool below_min = gt_op == GtOp::gt ? Util::less_equal(actual, min) : Util::less(actual, min);
  const bool above_max = lt_op == LtOp::lt ? Util::greater_equal(actual, max) : Util::greater(actual, max);
  return below_min || above_max;
}

template <typename A, typename E>
std::string BeBetween<A, E>::description() {
  return std::format("be between {} and {} ({})", min, max, (mode == RangeMode::exclusive ? "exclusive" : "inclusive"));
}

}  // namespace CppSpec::Matchers

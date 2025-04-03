/** @file */
#pragma once

#include "matchers/matcher_base.hpp"

namespace CppSpec::Matchers {

enum class RangeMode { exclusive, inclusive };

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
  std::string verb() override { return "be between"; }
  std::string description() override;
};

template <typename A, typename E>
bool BeBetween<A, E>::match() {
  auto actual = this->actual();
  bool result1;
  switch (gt_op) {
    case GtOp::gt:
      result1 = actual > min;
      break;
    case GtOp::gt_eq:
      result1 = actual >= min;
      break;
  }
  bool result2;
  switch (lt_op) {
    case LtOp::lt:
      result2 = actual < max;
      break;
    case LtOp::lt_eq:
      result2 = actual <= max;
      break;
  }
  return result1 && result2;
}

template <typename A, typename E>
std::string BeBetween<A, E>::description() {
  return std::format("be between {} and {} ({})", min, max, (mode == RangeMode::exclusive ? "exclusive" : "inclusive"));
}

}  // namespace CppSpec::Matchers

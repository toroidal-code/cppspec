#ifndef BE_BETWEEN_H
#define BE_BETWEEN_H

#include "basematcher.hpp"

namespace Matchers {
template <typename A, typename E>
class BeBetween : public BaseMatcher<A, E> {
  enum class RangeMode { exclusive, inclusive };
  E min;
  E max;
  RangeMode mode = RangeMode::inclusive;
  enum class LtOp { lt, lt_eq } lt_op;
  enum class GtOp { gt, gt_eq } gt_op;

 public:
  BeBetween(Expectations::Expectation<A> &expectation, E min, E max)
      : BaseMatcher<A, E>(expectation), min(min), max(max) {}

  bool inclusive();
  bool exclusive();

  bool match() override;
  std::string description() override;
};

/**
 * Makes the between comparison inclusive
 *
 * @example
 *   expect(4).to(be_between(2,3).inclusive())
 *
 * @note The matcher is inclusive by default; this simply
 *       provides a way to be more explicit about it.
 */
template <typename A, typename E>
bool BeBetween<A, E>::inclusive() {
  lt_op = LtOp::lt_eq;
  gt_op = GtOp::gt_eq;
  mode = RangeMode::inclusive;
  return this->run();
}

/**
 * Makes the between comparison exclusive
 *
 * @example
 *   expect(4).to(be_between(2,3).exclusive())
 */
template <typename A, typename E>
bool BeBetween<A, E>::exclusive() {
  lt_op = LtOp::lt;
  gt_op = GtOp::gt;
  mode = RangeMode::exclusive;
  return this->run();
}

template <typename A, typename E>
bool BeBetween<A, E>::match() {
  auto actual = this->get_actual();
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
  std::stringstream ss;
  ss << "be between " << min << " and " << max << " ("
     << (mode == RangeMode::exclusive ? "exclusive" : "inclusive") << ")";
  return ss.str();
}
}
#endif /* BE_BETWEEN_H */

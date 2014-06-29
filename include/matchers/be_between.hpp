#ifndef BE_BETWEEN_H
#define BE_BETWEEN_H

#include "basematcher.hpp"

namespace Matchers {
template <typename A, typename E>
class BeBetween : public BaseMatcher<A,E> {
  E min;
  E max;
  enum class Mode { exclusive, inclusive } mode;
  enum class LtOp { lt, lt_eq } lt_op;
  enum class GtOp { gt, gt_eq } gt_op;
 public:
  BeBetween(Expectations::Expectation<A> &expectation, 
            E min, E max) 
      : BaseMatcher<A,E>(expectation), min(min), max(max), 
        mode(Mode::inclusive), lt_op(LtOp::lt_eq), 
        gt_op(GtOp::gt_eq) {
    //static_assert(std::is_same<A,E>::value, "Expected and actual value's types are different");
  }
  virtual bool matches(A actual);
  virtual std::string description();
  BeBetween & inclusive();
  BeBetween & exclusive();
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
BeBetween<A,E> & BeBetween<A,E>::inclusive() {
  lt_op = LtOp::lt_eq;
  gt_op = GtOp::gt_eq;
  mode = Mode::inclusive;
  return *this;
}

/**
 * Makes the between comparison exclusive
 *
 * @example
 *   expect(4).to(be_between(2,3).exclusive())
 */
template <typename A, typename E>
BeBetween<A,E> & BeBetween<A,E>::exclusive() {
  lt_op = LtOp::lt;
  gt_op = GtOp::gt;
  mode = Mode::exclusive;
  return *this;
}

template <typename A, typename E> 
bool BeBetween<A,E>::matches(A actual) {
  this->actual = actual;
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
std::string BeBetween<A,E>::description() {
  std::stringstream ss;
  ss << "be between " << min << " and " << max << " (" << 
      (mode == Mode::exclusive ? "exclusive" : "inclusive") 
     << ")";
  return ss.str();
}

}
#endif /* BE_BETWEEN_H */

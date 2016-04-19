#ifndef IT_BASE_H
#define IT_BASE_H
#include "runnable.hpp"

/**
 * @brief Most base class for `it` expressions.
 *
 * This class is needed to prevent a circular dependency between it.hpp and
 * basematcher.hpp. Matchers need to know whether or not an `it` has an explicit
 * description string or whether the description should be generated. `it`s need
 * to be able to refer to Expectations, and Expectations need to know about
 * Matchers and execute them. This class is the least common denominator of the
 * `it` classes, and thus is used to resolve the dependency cycle.
 */
class ItBase : public Runnable {
  std::string descr = "";

 public:
  ItBase() = delete;

  /**
   * @brief Create an ItBase without an explicit description
   * @return the constructed ItBase
   */
  explicit ItBase(Child &parent) : Runnable(parent){};

  /**
   * @brief Create an ItBase with an explicit description.
   * @param descr the description of the `it` statement
   * @return the constructed ItBase
   */
  explicit ItBase(Child &parent, std::string descr)
      : Runnable(parent), descr(descr){};

  /**
   * @brief Get whether the object needs a description string
   * @return whether this object needs a description to be generated or not
   */
  bool needs_descr() { return descr.empty(); }

  /**
   * @brief Get the description string for the `it` statement
   * @return the description string
   */
  std::string get_descr() { return descr; }
};

#endif /* IT_BASE_H */

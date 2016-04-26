/** @file */
#ifndef CPPSPEC_IT_BASE_HPP
#define CPPSPEC_IT_BASE_HPP
#pragma once

#include <vector>
#include "runnable.hpp"
#include "let.hpp"
#include "util.hpp"
namespace CppSpec {

namespace Expectations {
template <typename T>
class Expectation;
}

/**
 * @brief Most-base class for `it` expressions.
 *
 * This class is needed to prevent a circular dependency between it.hpp and
 * basematcher.hpp. Matchers need to know whether or not an `it` has an explicit
 * description string or whether the description should be generated. `it`s need
 * to be able to refer to Expectations, and Expectations need to know about
 * Matchers and execute them. This class is the least common denominator of the
 * `it` classes, and thus is used to resolve the dependency cycle.
 */
class BaseIt : public Runnable {
  std::string descr = "";

 public:
  BaseIt() = delete;
  BaseIt(BaseIt const &copy) : Runnable(copy.get_parent()), descr(copy.descr) {}

  /**
   * @brief Create an BaseIt without an explicit description
   * @return the constructed BaseIt
   */
  explicit BaseIt(Child &parent) : Runnable(parent) {}

  /**
   * @brief Create an BaseIt with an explicit description.
   * @param descr the description of the `it` statement
   * @return the constructed BaseIt
   */
  explicit BaseIt(Child &parent, std::string descr)
      : Runnable(parent), descr(descr) {}

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
  const std::string get_descr() const { return descr; }
  BaseIt &set_descr(std::string descr);

  template <class U>
  typename std::enable_if<not Util::is_functional<U>::value,
                          Expectations::Expectation<U>>::type
  expect(U value);

  template <typename U>
  auto expect(U block) -> typename std::enable_if<
      Util::is_functional<U>::value,
      Expectations::Expectation<decltype(block())>>::type;

  template <class U>
  Expectations::Expectation<std::vector<U>> expect(
      std::initializer_list<U> init_list);

  template <class U>
  Expectations::Expectation<U> expect(Let<U> &let);
};

inline BaseIt &BaseIt::set_descr(std::string descr) {
  this->descr = descr;
  return *this;
}

}  // namespace CppSpec
#endif  // CPPSPEC_IT_BASE_HPP

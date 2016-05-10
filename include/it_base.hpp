/** @file */
#ifndef CPPSPEC_IT_BASE_HPP
#define CPPSPEC_IT_BASE_HPP
#pragma once

#include <string>
#include <vector>
#include "runnable.hpp"
#include "let.hpp"
#include "util.hpp"

namespace CppSpec {

template <typename T>
class ExpectationValue;
template <typename T>
class ExpectationFunc;

/**
 * @brief Base class for `it` expressions.
 *
 * This class is needed to prevent a circular dependency between it.hpp and
 * basematcher.hpp. Matchers need to know whether or not an `it` has an explicit
 * description string or whether the description should be generated. `it`s need
 * to be able to refer to Expectations, and Expectations need to know about
 * Matchers and execute them. This class is the least common denominator of the
 * `it` classes, and thus is used to resolve the dependency cycle.
 */
class ItBase : public Runnable {
  /** @brief The documentation string for this `it` */
  std::string description = "";

 public:
  ItBase() = delete;  // Don't allow a default constructor

  /** @brief Copy constructor */
  ItBase(const ItBase &copy) noexcept : Runnable(copy),
                                        description(copy.description) {}

  /**
   * @brief Create an BaseIt without an explicit description
   * @return the constructed BaseIt
   */
  explicit ItBase(const Child &parent) noexcept : Runnable(parent) {}

  /**
   * @brief Create an BaseIt with an explicit description.
   * @param description the documentation string of the `it` statement
   * @return the constructed BaseIt
   */
  explicit ItBase(const Child &parent, std::string description) noexcept
      : Runnable(parent),
        description(description) {}

  /**
   * @brief Get whether the object needs a description string
   * @return whether this object needs a description to be generated or not
   */
  const bool needs_description() noexcept { return description.empty(); }

  /**
   * @brief Get the description string for the `it` statement
   * @return the description string
   */
  std::string get_description() noexcept { return description; }
  const std::string get_description() const noexcept { return description; }

  /**
   * @brief Set the description string
   * @return a reference to the modified ItBase
   */
  ItBase &set_description(std::string description) noexcept {
    this->description = description;
    return *this;
  }

  /**
   * @brief The `expect` object generator for objects and LiteralTypes
   *
   * @param value the item to wrap. This parameter is passed by value so
   *              that statements like `expect(2)` or `expect(SomeClass())`
   *              can be passed in
   *
   * @tparam T the type of the object contained in the ExpectationValue
   *
   * @return a ExpectationValue object containing the given value.
   */
  template <typename T>
  typename std::enable_if<not Util::is_functional<T>::value,
                          ExpectationValue<T>>::type
  expect(T value);

  /**
   * @brief The `expect` object generator for lambdas
   *
   * @param block the lambda to wrap.
   *
   * @tparam T the type of the lambda/function contained in the ExpectationFunc
   *
   * @return a ExpectationFunc object containing the given value.
   */
  template <typename T>
  typename std::enable_if<Util::is_functional<T>::value,
                          ExpectationFunc<T>>::type
  expect(T block);

  /**
   * @brief The `expect` object generator for initializer lists
   *
   * @param init_list the list to wrap
   *
   * @tparam T the type of the items inside the initializer list
   *
   * @return a ExpectationValue object containing the given init_list.
   */
  template <typename T>
  ExpectationValue<std::initializer_list<T>> expect(
      std::initializer_list<T> init_list);

  /**
   * @brief The `expect` object generator for Let
   *
   * @param block the let variable
   *
   * @tparam T the type of the value contained in the ExpectationFunc
   *
   * @return a ExpectationValue object containing the given value.
   */
  template <typename T>
  ExpectationValue<T> expect(Let<T> &let);

  /**
   * @brief The `expect` object generator for const char*
   *
   * @param string the string to wrap
   * @return a ExpectationValue object containing a C++ string
   */
  ExpectationValue<std::string> expect(const char *string);
};

}  // namespace CppSpec
#endif  // CPPSPEC_IT_BASE_HPP

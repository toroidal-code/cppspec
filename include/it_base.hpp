/** @file */
#pragma once

#include <algorithm>
#include <functional>
#include <source_location>
#include <string>
#include <utility>

#include "let.hpp"
#include "runnable.hpp"
#include "util.hpp"

namespace CppSpec {

template <typename T>
class ExpectationValue;
template <Util::is_functional T>
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
  std::string description;
  std::list<Result> results{};  // The results of the `it` statement

 public:
  ItBase() = delete;  // Don't allow a default constructor

  /**
   * @brief Create an BaseIt without an explicit description
   * @return the constructed BaseIt
   */
  explicit ItBase(Runnable& parent, std::source_location location) noexcept : Runnable(parent, location) {}

  /**
   * @brief Create an BaseIt with an explicit description.
   * @param description the documentation string of the `it` statement
   * @return the constructed BaseIt
   */
  explicit ItBase(Runnable& parent, std::source_location location, const char* description) noexcept
      : Runnable(parent, location), description(std::move(description)) {}

  /**
   * @brief Get whether the object needs a description string
   * @return whether this object needs a description to be generated or not
   */
  bool needs_description() noexcept { return description.empty(); }

  /**
   * @brief Get the description string for the `it` statement
   * @return the description string
   */
  [[nodiscard]] std::string get_description() const noexcept { return description; }

  /**
   * @brief Set the description string
   * @return a reference to the modified ItBase
   */
  ItBase& set_description(std::string_view description) noexcept {
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
  template <Util::is_not_functional T>
  ExpectationValue<T> expect(T value, std::source_location location = std::source_location::current());

  /**
   * @brief The `expect` object generator for lambdas
   *
   * @param block the lambda to wrap.
   *
   * @tparam T the type of the lambda/function contained in the ExpectationFunc
   *
   * @return a ExpectationFunc object containing the given value.
   */
  template <Util::is_functional T>
  ExpectationFunc<T> expect(T block, std::source_location location = std::source_location::current());

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
  ExpectationValue<std::initializer_list<T>> expect(std::initializer_list<T> init_list,
                                                    std::source_location location = std::source_location::current());

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
  ExpectationValue<T> expect(Let<T>& let, std::source_location location = std::source_location::current());

  /**
   * @brief The `expect` object generator for const char*
   *
   * @param string the string to wrap
   * @return a ExpectationValue object containing a C++ string
   */
  ExpectationValue<std::string> expect(const char* string,
                                       std::source_location location = std::source_location::current());

  void add_result(const Result& result) { results.push_back(result); }
  std::list<Result>& get_results() noexcept { return results; }
  const std::list<Result>& get_results() const noexcept { return results; }
  void clear_results() noexcept { results.clear(); }

  [[nodiscard]] Result get_result() const override {
    if (results.empty()) {
      return Result::success(this->get_location());
    }

    return *std::ranges::fold_left_first(results, std::logical_and<>{});
  }
};

}  // namespace CppSpec

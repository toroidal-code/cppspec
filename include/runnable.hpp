/**
 * @file
 */
#ifndef CPPSPEC_RUNNABLE_HPP
#define CPPSPEC_RUNNABLE_HPP
#pragma once

#include "child.hpp"
#include "result.hpp"

namespace CppSpec {

/**
 * @brief Abstract base class for executable objects
 */
class Runnable : public Child {
 public:
  // Default constructor/destructor
  Runnable() = default;
  virtual ~Runnable() = default;

  // Move constructor/operator
  Runnable(Runnable &&) = default;
  Runnable &operator=(Runnable &&) = default;

  // Copy constructor/operator
  Runnable(const Runnable &parent) = default;
  Runnable &operator=(const Runnable &) = delete;

  // WARNING! Be *very* carful about calling this constructor.
  // You *need* to make sure that the argument is a Child and not a
  // subclass of Runnable. Otherwise you'll end up calling the
  // copy constructor on accident.
  explicit Runnable(const Child &parent) noexcept : Child(Child::of(parent)) {}

  // Interface function
  virtual Result run(Formatters::BaseFormatter &printer) = 0;
};

}  // namespace CppSpec
#endif  // CPPSPEC_RUNNABLE_HPP

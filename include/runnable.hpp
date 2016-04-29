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
  Runnable() = default;
  explicit Runnable(Child &parent) noexcept : Child(parent) {}
  explicit Runnable(Child *parent) noexcept : Child(parent) {}
  explicit Runnable(const Child *parent) noexcept : Child(parent) {}
  virtual Result run(Formatters::BaseFormatter &printer) = 0;
};

}  // namespace CppSpec
#endif  // CPPSPEC_RUNNABLE_HPP

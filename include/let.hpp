/** @file */
#ifndef CPPSPEC_LET_HPP
#define CPPSPEC_LET_HPP
#pragma once

#include <functional>
#include "optional/optional.hpp"

namespace CppSpec {

class LetBase {
 protected:
  bool delivered;

 public:
  LetBase() noexcept : delivered(false) {}
  LetBase(const LetBase &copy) = default;
  constexpr void reset() noexcept { delivered = false; }
  constexpr const bool has_result() noexcept { return this->delivered; }
  constexpr const bool has_result() const noexcept { return this->delivered; }
};

template <typename T>
class Let : public LetBase {
  typedef std::function<T()> block_t;
  std::experimental::optional<T> result;

  block_t body;

  void exec();

 public:
  explicit Let(block_t body) noexcept : LetBase(), body(body) {}

  T *operator->() {
    value();
    return result.operator->();
  }

  constexpr T &operator*() & { return value(); }
  constexpr const T &operator*() const & { return value(); }

  T &value()&;
};

/** @brief Executes the block of the let statment */
template <typename T>
void Let<T>::exec() {
  if (!delivered) {
    result = body();
    delivered = true;
  }
}

/**
 * @brief Get the value contained in the Let
 * @return a reference to the returned object of the let statement
 */
template <typename T>
T &Let<T>::value() & {
  exec();
  return result.value();
}

}  // namespace CppSpec
#endif  // CPPSPEC_LET_HPP

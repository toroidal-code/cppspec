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
  LetBase() : delivered(false) {}
  LetBase(const LetBase &copy) = default;
  void reset() { delivered = false; }
  bool has_result() { return delivered; }
};

template <typename T>
class Let : public LetBase {
  typedef std::function<T()> block_t;
  std::experimental::optional<T> result;

  block_t body;

 public:
  explicit Let(block_t body) : LetBase(), body(body) {}

  T *operator->() {
    value();
    return result.operator->();
  }

  T &operator*() & { return value(); }
  void exec();
  T &value()&;
};

template <typename T>
void Let<T>::exec() {
  if (!delivered) {
    result = body();
    delivered = true;
  }
}

template <typename T>
T &Let<T>::value() & {
  exec();
  return result.value();
}

}  // namespace CppSpec
#endif  // CPPSPEC_LET_HPP

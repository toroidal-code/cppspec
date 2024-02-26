/** @file */
#pragma once

#include <functional>
#include <optional>

namespace CppSpec {

/**
 * @brief Base class for lets to abstract away the template arguments
 *
 * Expectation needs to know whether the calculated value of a
 * Let has been delivered or not, but doesn't need to know the value
 * itself or its type.
 */
class LetBase {
 protected:
  bool delivered{false};

 public:
  constexpr LetBase() noexcept = default;
  LetBase(const LetBase &copy) = default;
  void reset() noexcept { delivered = false; }
  [[nodiscard]] constexpr bool has_result() const noexcept { return this->delivered; }
};

/**
 * @brief A container that memoizes the result of a block in `it's
 *
 * @tparm T the type of the object that the Let will contain, aka
 *          return type of the block.
 */
template <typename T>
class Let : public LetBase {
  using block_t = std::function<T()>;
  std::optional<T> result;

  block_t body;

  void exec();

 public:
  explicit Let(block_t body) noexcept : LetBase(), body(body) {}

  T *operator->() {
    value();
    return result.operator->();
  }

  T &operator*() & { return value(); }
  T &value() &;
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

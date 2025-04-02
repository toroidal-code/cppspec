#pragma once
#include <expected>

#include "matchers/matcher_base.hpp"

namespace CppSpec::Matchers {

template <typename T>
concept expected = requires(T t) {
  { t.error() } -> std::same_as<typename T::error_type &>;
};

template <expected T>
class HaveError : public MatcherBase<T, void *> {
 public:
  HaveError(Expectation<T> &expectation) : MatcherBase<T, void *>(expectation) {}

  std::string verb() override { return "have an error"; }
  std::string description() override { return verb(); }

  bool match() override { return (!this->actual().has_value()); }
};

template <expected T, typename E>
class HaveErrorEqualTo : public MatcherBase<T, E> {
 public:
  static_assert(std::is_same_v<typename T::error_type, E>, "the contained error_type must match the expected type");
  HaveErrorEqualTo(Expectation<T> &expectation, E expected) : MatcherBase<T, E>(expectation, expected) {}

  bool match() { return (this->actual().error() == this->expected()); }
};

}  // namespace CppSpec::Matchers

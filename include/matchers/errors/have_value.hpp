#pragma once
#include <optional>

#include "matchers/matcher_base.hpp"

namespace CppSpec::Matchers {

template <typename T>
concept optional = requires(T t) {
  { t.has_value() } -> std::same_as<bool>;
};

template <optional T>
class HaveValue : public MatcherBase<T, void *> {
 public:
  HaveValue(Expectation<T> &expectation) : MatcherBase<T, void *>(expectation) {}

  std::string description() override {return "have a value"; }

  bool match() override { return (this->actual().has_value()); }
};

template <optional T, typename E>
class HaveValueEqualTo : public Equal<T, E> {
 public:
  static_assert(std::is_same_v<typename T::value_type, E>,
                "the contained value_type must match the expected type");
  HaveValueEqualTo(Expectation<T> &expectation, E expected) : Equal<T, E>(expectation, expected) {}

  bool match() { return (this->actual().value() == this->expected()); }
};

}  // namespace CppSpec::Matchers

/** @file */
#ifndef CPPSPEC_IT_HPP
#define CPPSPEC_IT_HPP
#pragma once

#include <string>
#include <vector>
#include "expectations/expectation.hpp"

namespace CppSpec {

/**
 * @brief
 */
class ItD : public ItBase {
 public:
  using Block = std::function<void(ItD &)>;

 private:
  /** @brief The block contained in the ItD */
  const Block block;

 public:
  ItD(const Child &parent, std::string description, Block block)
      : ItBase(parent, description), block(block) {}

  ItD(const Child &parent, Block block) : ItBase(parent), block(block) {}

  Result run(Formatters::BaseFormatter &printer) override;
};

/**
 * @brief
 * @tparam T
 */
template <typename T>
class ItCD : public ItBase {
 public:
  using Block = std::function<void(ItCD<T> &)>;

 private:
  /** @brief The block contained in the ItCD */
  const Block block;

 public:
  /**
   * @brief A reference to the parent ClassDescription's subject
   *
   * Public so that we can easily do expect(subject) without
   * putting getters in the macro-expansion
   */
  const T &subject;

  // This is only ever instantiated by ClassDescription<T>
  ItCD(const Child &parent, const T &subject, std::string description, Block block)
      : ItBase(parent, description), block(block), subject(subject) {}

  ItCD(const Child &parent, const T &subject, Block block)
      : ItBase(parent), block(block), subject(subject) {}

  Expectations::ExpectationValue<T> is_expected();
  Result run(Formatters::BaseFormatter &printer) override;
};

/**
 * @brief A simple expect statement
 *
 * @code
 *   expect(1)
 *   expect(true)
 *   expect([] -> int { return 4; })
 * @endcode
 */
template <class T>
typename std::enable_if<not Util::is_functional<T>::value,
                        Expectations::ExpectationValue<T>>::type
ItBase::expect(T value) {
  return Expectations::ExpectationValue<T>(*this, value);
}

template <typename T>
auto ItBase::expect(T block) ->
    typename std::enable_if<Util::is_functional<T>::value,
                            Expectations::ExpectationFunc<T>>::type {
  return Expectations::ExpectationFunc<T>(*this, block);
}

template <typename T>
Expectations::ExpectationValue<T> ItBase::expect(Let<T> &let) {
  return Expectations::ExpectationValue<T>(*this, let.value());
}

/**
 * @brief An expect for initializer_list subjects
 *
 * @code
 *   expect({1,2,3})
 * @endcode
 */
template <class T>
Expectations::ExpectationValue<std::initializer_list<T>> ItBase::expect(
    std::initializer_list<T> init_list) {
  return Expectations::ExpectationValue<std::initializer_list<T>>(*this,
                                                                  init_list);
}

inline Expectations::ExpectationValue<std::string> ItBase::expect(
    const char *str) {
  return Expectations::ExpectationValue<std::string>(*this, std::string(str));
}

}  // namespace CppSpec
#endif  // CPPSPEC_IT_HPP

/** @file */
#ifndef CPPSPEC_IT_HPP
#define CPPSPEC_IT_HPP
#pragma once

#include <string>
#include <vector>
#include "expectations/expectation.hpp"

namespace CppSpec {

class ItD : public ItBase {
  std::function<void(ItD &)> body;

 public:
  ItD(Child &parent, std::string descr, std::function<void(ItD &)> body)
      : ItBase(parent, descr), body(body) {}

  ItD(Child &parent, std::function<void(ItD &)> body)
      : ItBase(parent), body(body) {}

  Result run(Formatters::BaseFormatter &printer) override;
};

template <class T>
class ItCd : public ItBase {
  std::function<void(ItCd<T> &)> body;

 public:
  T &subject;  // reference to parent ClassDescription's subject

  // This is only ever instantiated by ClassDescription<T>
  ItCd(Child &parent, T &subject, std::string descr,
       std::function<void(ItCd<T> &)> body)
      : ItBase(parent, descr), body(body), subject(subject) {}

  ItCd(Child &parent, T &subject, std::function<void(ItCd<T> &)> body)
      : ItBase(parent), body(body), subject(subject) {}

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
auto ItBase::expect(T block) -> typename std::enable_if<
    Util::is_functional<T>::value,
    Expectations::ExpectationFunc<T>>::type
{
  return Expectations::ExpectationFunc<T>(*this, block);
}

// template <class T>
// Expectations::Expectation<T> ItExpBase::expect(T const &value) {
//   Expectations::Expectation<T> expectation(value);
//   expectation.set_parent(this);
//   return expectation;
// }

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
Expectations::ExpectationValue<std::vector<T>> ItBase::expect(
    std::initializer_list<T> init_list) {
  return Expectations::ExpectationValue<std::vector<T>>(*this, init_list);
}

inline Expectations::ExpectationValue<std::string> ItBase::expect(const char * str) {
  return Expectations::ExpectationValue<std::string>(*this, std::string(str));
}


}  // namespace CppSpec
#endif  // CPPSPEC_IT_HPP

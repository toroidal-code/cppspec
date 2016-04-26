/** @file */
#ifndef CPPSPEC_IT_HPP
#define CPPSPEC_IT_HPP
#pragma once

#include <string>
#include <vector>
#include "expectations/expectation.hpp"

namespace CppSpec {

class ItD : public BaseIt {
  std::function<void(ItD &)> body;

 public:
  ItD(Child &parent, std::string descr, std::function<void(ItD &)> body)
      : BaseIt(parent, descr), body(body) {}

  ItD(Child &parent, std::function<void(ItD &)> body)
      : BaseIt(parent), body(body) {}

  Result run(Formatters::BaseFormatter &printer) override;
};

template <class T>
class ItCd : public BaseIt {
  std::function<void(ItCd<T> &)> body;

 public:
  T &subject;  // reference to parent ClassDescription's subject

  // This is only ever instantiated by ClassDescription<T>
  ItCd(Child &parent, T &subject, std::string descr,
       std::function<void(ItCd<T> &)> body)
      : BaseIt(parent, descr), body(body), subject(subject) {}

  ItCd(Child &parent, T &subject, std::function<void(ItCd<T> &)> body)
      : BaseIt(parent), body(body), subject(subject) {}

  Expectations::Expectation<T> is_expected();
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
                        Expectations::Expectation<T>>::type
BaseIt::expect(T value) {
  return Expectations::Expectation<T>(*this, value);
}

template <typename T>
auto BaseIt::expect(T block) ->
    typename std::enable_if<Util::is_functional<T>::value,
                            Expectations::Expectation<decltype(block())>>::type

{
  return expect(block());
}

// template <class T>
// Expectations::Expectation<T> ItExpBase::expect(T const &value) {
//   Expectations::Expectation<T> expectation(value);
//   expectation.set_parent(this);
//   return expectation;
// }

template <typename T>
Expectations::Expectation<T> BaseIt::expect(Let<T> &let) {
  return Expectations::Expectation<T>(*this, let.value());
}

/**
 * @brief An expect for initializer_list subjects
 *
 * @code
 *   expect({1,2,3})
 * @endcode
 */
template <class T>
Expectations::Expectation<std::vector<T>> BaseIt::expect(
    std::initializer_list<T> init_list) {
  return Expectations::Expectation<std::vector<T>>(*this, init_list);
}

}  // namespace CppSpec
#endif  // CPPSPEC_IT_HPP

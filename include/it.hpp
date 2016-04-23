#ifndef CPPSPEC_IT_HPP
#define CPPSPEC_IT_HPP
#include "expectations/expectation.hpp"

namespace CppSpec {

class ItExpBase : public BaseIt {
 public:
  ItExpBase(ItExpBase const &copy) : BaseIt(copy){};
  ItExpBase(Child &parent) : BaseIt(parent){};
  ItExpBase(Child &parent, std::string descr) : BaseIt(parent, descr){};
  template <class U>
  typename std::enable_if<not Util::is_functional<U>::value,
                          Expectations::Expectation<U>>::type
  expect(U value);

  template <typename U>
  auto expect(U block) -> typename std::enable_if<
      Util::is_functional<U>::value,
      Expectations::Expectation<decltype(block())>>::type;

  template <class U>
  Expectations::Expectation<std::vector<U>> expect(
      std::initializer_list<U> init_list);

  template <class U>
  Expectations::Expectation<U> expect(Let<U> let);
};

class ItD : public ItExpBase {
  std::function<void(ItD &)> body;

 public:
  ItD(Child &parent, std::string descr, std::function<void(ItD &)> body)
      : ItExpBase(parent, descr), body(body) {}

  ItD(Child &parent, std::function<void(ItD &)> body)
      : ItExpBase(parent), body(body) {}

  Result run(BasePrinter &printer) override;
};

// This is cannot be instantiated.
template <class T>
class ItCd : public ItExpBase {
  std::function<void(ItCd<T> &)> body;

 public:
  T &subject;  // reference to parent ClassDescription's subject

  // This is only ever instantiated by ClassDescription<T>
  ItCd(Child &parent, T &subject, std::string descr,
       std::function<void(ItCd<T> &)> body)
      : ItExpBase(parent, descr), body(body), subject(subject) {}

  ItCd(Child &parent, T &subject, std::function<void(ItCd<T> &)> body)
      : ItExpBase(parent), body(body), subject(subject) {}

  Expectations::Expectation<T> is_expected();
  Result run(BasePrinter &printer) override;
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
ItExpBase::expect(T value) {
  return Expectations::Expectation<T>(*this, value);
}

template <typename T>
auto ItExpBase::expect(T block) ->
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

/**
 * @brief An expect for initializer_list subjects
 *
 * @code
 *   expect({1,2,3})
 * @endcode
 */
template <class T>
Expectations::Expectation<std::vector<T>> ItExpBase::expect(
    std::initializer_list<T> init_list) {
  Expectations::Expectation<std::vector<T>> expectation(*this, init_list);
  return expectation;
}

} // ::CppSpec
#endif // CPPSPEC_IT_HPP

#ifndef IT_H
#define IT_H
#include "expectations/expectation.hpp"

class ItExpBase : public ItBase {
 public:
  ItExpBase() : ItBase(){};
  ItExpBase(std::string descr) : ItBase(descr){};
  template <class U>
  Expectations::Expectation<U> expect(U value);

  // template <class U>
  // Expectations::Expectation<U> expect(U &value);

  template <class U>
  Expectations::Expectation<std::vector<U>> expect(
      std::initializer_list<U> init_list);

  template <class U>
  Expectations::Expectation<U> expect(Let<U> let);
};

class ItD : public ItExpBase {
  std::function<void(ItD &)> body;

 public:
  ItD(std::string descr, std::function<void(ItD &)> body)
      : ItExpBase(descr), body(body) {}

  ItD(std::function<void(ItD &)> body) : body(body) {}

  bool run();
};

// This is cannot be instantiated.
template <class T>
class ItCd : public ItExpBase {
  std::function<void(ItCd<T> &)> body;

 public:
  T &subject;

  // This is only ever instantiated by ClassDescription<T>
  ItCd(T &subject, std::string descr, std::function<void(ItCd<T> &)> body)
      : ItExpBase(descr), body(body), subject(subject) {}

  ItCd(T &subject, std::function<void(ItCd<T> &)> body)
      : body(body), subject(subject) {}

  Expectations::Expectation<T> is_expected();
  bool run();
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
Expectations::Expectation<T> ItExpBase::expect(T value) {
  Expectations::Expectation<T> expectation(value);
  expectation.set_parent(this);
  return expectation;
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
  Expectations::Expectation<std::vector<T>> expectation(init_list);
  expectation.set_parent(this);
  return expectation;
}

#endif /* IT_H */

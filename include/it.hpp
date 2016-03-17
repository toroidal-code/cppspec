#ifndef IT_H
#define IT_H
#include <string>
#include <functional>
#include "expectations/expectation.hpp"

class Description;

template <class T>
class ClassDescription;

class ItExpBase : public ItBase {
 public:
  ItExpBase() : ItBase(){};
  ItExpBase(std::string descr) : ItBase(descr){};
  template <class U>
  Expectations::Expectation<U> expect(U value);

  template <class U>
  Expectations::Expectation<std::vector<U>> expect(
      std::initializer_list<U> init_list);
};

class ItD : public ItExpBase {
  std::function<void(ItD &)> body;

 public:
  ItD(std::string descr, std::function<void(ItD &)> body)
      : ItExpBase(descr), body(body) {}

  ItD(std::function<void(ItD &)> body) : body(body) {
    this->set_needs_descr(true);
  }

  bool run();
};

// This is cannot be instantiated.
template <class T>
class ItCd : public ItExpBase {
  std::function<void(ItCd<T> &)> body;

 public:
  // This is only ever instantiated by ClassDescription<T>
  ItCd(std::string descr, std::function<void(ItCd<T> &)> body)
      : ItExpBase(descr), body(body) {}

  ItCd(std::function<void(ItCd<T> &)> body) : body(body) {
    this->set_needs_descr(true);
  }

  Expectations::Expectation<T> is_expected();
  bool run();
};

/**
   * expect(1)
   * expect(true)
   * expect([] -> int { return 4; })
   */
template <class T>
Expectations::Expectation<T> ItExpBase::expect(T value) {
  Expectations::Expectation<T> expectation(value);
  expectation.set_parent(this);
  return expectation;
}

/**
 * expect({1,2,3})
 */
template <class T>
Expectations::Expectation<std::vector<T>> ItExpBase::expect(
    std::initializer_list<T> init_list) {
  Expectations::Expectation<std::vector<T>> expectation(init_list);
  expectation.set_parent(this);
  return expectation;
}

template <class T>
Expectations::Expectation<T> ItCd<T>::is_expected() {
  ClassDescription<T> *cd =
      static_cast<ClassDescription<T> *>(this->get_parent());
  Expectations::Expectation<T> expectation(cd->get_subject());
  expectation.set_parent(this);
  return expectation;
}

bool ItD::run() {
  if (!this->needs_descr()) {
    std::cout << padding() << descr << std::endl;
  }
  body(*this);
  return this->get_status();
}

template <class T>
bool ItCd<T>::run() {
  if (!this->needs_descr()) {
    std::cout << padding() << descr << std::endl;
  }
  body(*this);
  return this->get_status();
}

#endif /* IT_H */

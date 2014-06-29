#ifndef BE_H
#define BE_H
#include <string>
#include <sstream>
#include "basematcher.hpp"
#include "pretty_matchers.hpp"

namespace Matchers {

template <class Derived>
class BeHelpers{
protected:
  virtual ~BeHelpers() {}
  std::string parenthesize(std::string string) { return "(" + string + ")"; }
  std::string expected_to_sentance() { 
    auto expected = static_cast<Derived*>(this)->get_expected();
    return static_cast<Derived*>(this)->split_words(expected);
  }
};


/********************
 *    Be matchers    *
 ********************/

/**
 * The basic `be` matcher
 * Not intended to be instantiated directly
 */
template <typename A, typename E>
class Be : public BaseMatcher<A,E>, BeHelpers<Be<A,E>> {
 public:
  Be(Expectations::Expectation<A> expectation) 
      : BaseMatcher<A,E>(expectation) {}
  virtual std::string failure_message();
  virtual std::string failure_message_when_negated();
  virtual bool match(E expected, A actual);
};

template <typename A, typename E>
std::string Be<A,E>::failure_message() {
  std::stringstream ss;
  ss <<"expected " << BaseMatcher<A,E>::get_actual() << " to evaluate to true";
  return ss.str();
}

template <typename A, typename E>
std::string Be<A,E>::failure_message_when_negated() {
  std::stringstream ss;
  ss << "expected " << BaseMatcher<A,E>::get_actual() << " to evaluate to false";
  return ss.str();
}

template <typename A, typename E>
bool Be<A,E>::match(E, A actual){
  return static_cast<bool>(actual);
}

template<class A>
class BeNullptr : BaseMatcher<A,std::nullptr_t> {
 public:
  BeNullptr(Expectations::Expectation<A> expectation) 
      : BaseMatcher<A,std::nullptr_t>(expectation) {}
  virtual ~BeNullptr();
  virtual std::string failure_message() override;
  virtual std::string failure_message_when_negated() override;
  virtual bool match(std::nullptr_t expected, A actual) override;
};


template<class T>
std::string BeNullptr<T>::failure_message() {
  return "expected: nullptr\n     got: " + BaseMatcher<T,T>::get_actual();
}

template<class T>
std::string BeNullptr<T>::failure_message_when_negated() {
  return  "expected: not nullptr\n     got: nullptr";
}

template<class A>
bool BeNullptr<A>::match(std::nullptr_t, A actual) {
  return actual == nullptr;
}

}


#endif /* BE_H */

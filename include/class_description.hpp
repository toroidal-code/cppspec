#ifndef CLASS_DESCRIPTION_H
#define CLASS_DESCRIPTION_H

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <iostream>
#include "util.hpp"
#include "description.hpp"
#include "it.hpp"
#include "expectations/expectation.hpp"

template <class T>
class ClassDescription : public Description {
  typedef std::function<void(ClassDescription<T>&)> block_t;
  T example;

 public:
  // Constructor
  // if there's no explicit subject given, then default using
  // the default constructor of the given type as the subject.
  ClassDescription<T>(block_t body)
      : Description(Util::polymorphic_to_string(T()) + " : " +
                    Util::demangle(typeid(T).name())),
        example(T()) {
    body(*this);
  };

  ClassDescription(T subject, block_t body)
      : Description(Util::polymorphic_to_string(subject) + " : " +
                    Util::demangle(typeid(T).name())),
        example(subject) {
    body(*this);
  };

  ClassDescription(T& subject, block_t body)
      : Description(Util::polymorphic_to_string(subject) + " : " +
                    Util::demangle(typeid(T).name())),
        example(subject) {
    body(*this);
  };

  template <typename U>
  ClassDescription(std::initializer_list<U> init_list, block_t body)
      : Description(Util::polymorphic_to_string(init_list) + " : " +
                    Util::demangle(typeid(T).name())),
        example(T(init_list)) {
    body(*this);
  };

  ClassDescription<T>(Description& d) : Description(d){};

  const bool has_subject = true;
  void set_subject(T& subject) { example = subject; }
  T& get_subject() { return example; }

  ItCd<T>& it(std::string descr, std::function<void(ItCd<T>&)> body);
  ItCd<T>& it(std::function<void(ItCd<T>&)> body);
  ClassDescription<T>& context(T subject, block_t body);
  ClassDescription<T>& context(T& subject, block_t body);
  ClassDescription<T>& context(block_t body);
  void before(std::string descr, block_t body);

  template <class U>
  ClassDescription<U> subject(U subject);

  template <class U>
  ClassDescription<U> subject(U& subject);

  static ClassDescription cast(Runnable r);
};

template <class T>
using ClassContext = ClassDescription<T>;

template <class T>
ClassContext<T>& ClassDescription<T>::context(
    T subject, std::function<void(ClassDescription&)> body) {
  Context* c = new ClassContext<T>(subject, body);
  c->set_parent(this);
  tasks.push_back(c);
  return *c;
}

template <class T>
ClassContext<T>& ClassDescription<T>::context(
    T& subject, std::function<void(ClassDescription&)> body) {
  return context(subject, body);
}

template <class T>
ClassContext<T>& ClassDescription<T>::context(
    std::function<void(ClassDescription&)> body) {
  ClassContext<T>* c = new ClassContext<T>(body);
  c->set_parent(this);
  tasks.push_back(c);
  return *c;
}

template <class T>
ClassContext<T>& Description::context(
    T subject, std::function<void(ClassDescription<T>&)> body) {
  ClassContext<T>* c = new ClassDescription<T>(body);
  c->set_subject(subject);
  c->set_parent(this);
  tasks.push_back(c);
  return *c;
}

// template <class T>
// ClassContext<T>& Description::context(
//     T& subject, std::function<void(ClassDescription<T>&)> body) {
//   return context(subject, body);
// }

template <class T, typename U>
ClassContext<T>& Description::context(
    std::initializer_list<U> init_list,
    std::function<void(ClassDescription<T>&)> body) {
  ClassContext<T>* c = new ClassContext<T>(T(init_list), body);
  c->set_parent(this);
  tasks.push_back(c);
  return *c;
}

template <class T>
ItCd<T>& ClassDescription<T>::it(std::string name,
                                 std::function<void(ItCd<T>&)> body) {
  ItCd<T>* it = new ItCd<T>(name, body);
  it->set_parent(this);
  tasks.push_back(it);
  return *it;
}

template <class T>
ItCd<T>& ClassDescription<T>::it(std::function<void(ItCd<T>&)> body) {
  ItCd<T>* it = new ItCd<T>(body);
  it->set_parent(this);
  tasks.push_back(it);
  return *it;
}

template <class T>
template <class U>
ClassDescription<U> ClassDescription<T>::subject(U subject) {
  ClassDescription<U> cd(static_cast<Description>(this));
  cd->example = subject;
  return cd;
}

template <class T>
template <class U>
ClassDescription<U> ClassDescription<T>::subject(U& subject) {
  ClassDescription<U> cd(static_cast<Description>(this));
  cd->example = subject;
  return cd;
}

#endif /* CLASS_DESCRIPTION_H */

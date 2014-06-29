#ifndef CLASS_DESCRIPTION_H
#define CLASS_DESCRIPTION_H

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <iostream>

#include "description.hpp"
#include "util.hpp"
#include "it.hpp"
#include "expectations/expectation.hpp"

template <class T>
class ClassDescription : public Description {
  typedef std::function<void(ClassDescription&)> block_t;
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
  T& get_subject() { return example; }

  It& it(std::string descr, std::function<void(It&)> body);
  It& it(std::function<void(It&)> body);
  ClassDescription<T>& context(T subject, block_t body);
  ClassDescription<T>& context(T& subject, block_t body);
  ClassDescription<T>& context(block_t body);
  void before(std::string descr, block_t body);

  template <class U>
  ClassDescription<U> subject(U subject);

  template <class U>
  ClassDescription<U> subject(U& subject);
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
  Context* c = new ClassContext<T>(body);
  c->set_parent(this);
  tasks.push_back(c);
  return *c;
}

template <class T>
It& ClassDescription<T>::it(std::string name, std::function<void(It&)> body) {
  It* it = new It(name, body);
  it->set_parent(this);
  tasks.push_back(it);
  return *it;
}

template <class T>
It& ClassDescription<T>::it(std::function<void(It&)> body) {
  It* it = new It(body);
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

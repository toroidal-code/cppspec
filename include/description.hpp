#ifndef DESCRIPTION_H
#define DESCRIPTION_H

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <iostream>
#include <memory>
#include "runnable.hpp"
#include "it.hpp"

template <class T>
class ClassDescription;

class Description : public Runnable {
  typedef std::function<void(Description &)> block_t;
  block_t body;

 protected:
  std::string descr = "";
  // std::vector<block_t> before_all;
  // std::vector<block_t> after_all;
  // std::vector<block_t> before_each;
  // std::vector<block_t> after_each;

  explicit Description(std::string descr) : descr(descr){};

 public:
  // Constructor
  Description(std::string descr, block_t body) : body(body), descr(descr){};

  const bool has_subject = false;

  // Spec functions
  bool it(std::string descr, std::function<void(ItD &)> body);
  bool it(std::function<void(ItD &)> body);
  bool context(std::string descr, block_t body);

  template <class T>
  bool context(T subject, std::function<void(ClassDescription<T> &)> body);

  template <class T>
  bool context(T &subject, std::function<void(ClassDescription<T> &)> body);

  template <class T, typename U>
  bool context(std::initializer_list<U> init_list,
               std::function<void(ClassDescription<T> &)> body);

  void before(std::string descr, block_t body);

  template <class T>
  ClassDescription<T> subject(T subject);

  template <class T>
  ClassDescription<T> subject(T &subject);

  template <class T>
  ClassDescription<std::vector<T>> subject(std::initializer_list<T> init_list);

  bool run();
};

typedef Description Context;

bool Description::context(std::string name,
                          std::function<void(Description &)> body) {
  Context context(name, body);
  context.set_parent(this);
  return context.run();
}

bool Description::it(std::string name, std::function<void(ItD &)> body) {
  ItD it(name, body);
  it.set_parent(this);
  return it.run();
}

bool Description::it(std::function<void(ItD &)> body) {
  ItD it(body);
  it.set_parent(this);
  return it.run();
}

template <class U>
ClassDescription<U> Description::subject(U subject) {
  ClassDescription<U> cd(this);
  cd->example = subject;
  return cd;
}

template <class U>
ClassDescription<U> Description::subject(U &subject) {
  ClassDescription<U> cd(this);
  cd->example = subject;
  return cd;
}

template <class U>
ClassDescription<std::vector<U>> Description::subject(
    std::initializer_list<U> init_list) {
  ClassDescription<U> cd(this);
  cd->example = std::vector<U>(init_list);
  return cd;
}

bool Description::run() {
  std::cout << padding() << descr << std::endl;
  body(*this);
  std::cout << std::endl;
  return this->get_status();
}

#endif /* DESCRIPTION_H */

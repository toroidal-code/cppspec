#ifndef DESCRIPTION_H
#define DESCRIPTION_H

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <iostream>
#include "runnable.hpp"
#include "it.hpp"

template <class T>
class ClassDescription;

class Description : public Runnable {
  typedef std::function<void(Description &)> block_t;

 protected:
  std::string descr = "";
  std::vector<block_t> befores;
  std::vector<block_t> afters;
  std::vector<Runnable *> tasks;

  explicit Description(std::string descr) : descr(descr){};

 public:
  // Constructor
  Description(std::string descr, block_t body) : descr(descr) { body(*this); };

  const bool has_subject = false;

  // Spec functions
  ItD &it(std::string descr, std::function<void(ItD &)> body);
  ItD &it(std::function<void(ItD &)> body);
  Description &context(std::string descr, block_t body);

  template <class T>
  ClassDescription<T> &context(std::string descr, block_t body);

  void before(std::string descr, block_t body);

  template <class T>
  ClassDescription<T> subject(T subject);

  template <class T>
  ClassDescription<T> subject(T &subject);

  bool run();
};

typedef Description Context;

Context &Description::context(std::string name,
                              std::function<void(Description &)> body) {
  Context *c = new Context(name, body);
  c->set_parent(this);
  tasks.push_back(c);
  return *c;
}

ItD &Description::it(std::string name, std::function<void(ItD &)> body) {
  ItD *it = new ItD(name, body);
  it->set_parent(this);
  tasks.push_back(it);
  return *it;
}

ItD &Description::it(std::function<void(ItD &)> body) {
  ItD *it = new ItD(body);
  it->set_parent(this);
  tasks.push_back(it);
  return *it;
}

bool Description::run() {
  std::cout << padding() << descr << std::endl;
  for (Runnable *task : tasks) {
    (*task).run();
  }
  std::cout << std::endl;
  return this->get_status();
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

#endif /* DESCRIPTION_H */

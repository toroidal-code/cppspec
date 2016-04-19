/**
 * @file
 * @brief Defines the Description class and associated functions
 */
#ifndef DESCRIPTION_H
#define DESCRIPTION_H
#include <queue>
#include <unordered_map>
#include "it.hpp"

template <class T>
class ClassDescription;

class Description : public Runnable {
  typedef std::function<void(Description &)> block_t;
  typedef std::function<void()> rule_block_t;
  block_t body;

 protected:
  std::string descr = "";
  std::deque<rule_block_t> after_alls;
  std::deque<rule_block_t> before_eaches;
  std::deque<rule_block_t> after_eaches;
  std::unordered_map<std::string, Runnable *> lets;

  Description(){};
  Description(std::string descr) : descr(descr){};
  Description(Child &parent, std::string descr, block_t body)
      : Runnable(parent), body(body), descr(descr){};

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

  //  template <class T>
  //  bool context(T &subject, std::function<void(ClassDescription<T> &)> body);

  template <class T, typename U>
  bool context(std::initializer_list<U> init_list,
               std::function<void(ClassDescription<T> &)> body);

  //  template <class T>
  //  ClassDescription<T> subject(T subject);
  //
  //  template <class T>
  //  ClassDescription<T> subject(T &subject);
  //
  //  template <class T>
  //  ClassDescription<std::vector<T>> subject(std::initializer_list<T>
  //  init_list);

  void before_each(rule_block_t block);
  void before_all(rule_block_t block);
  void after_each(rule_block_t block);
  void after_all(rule_block_t block);
  void exec_before_eaches();
  void exec_after_eaches();

  void let(std::string, Runnable &body);
  bool run();
  void reset_lets();
  Runnable *find_let(std::string);
};

typedef Description Context;

bool Description::context(std::string name,
                          std::function<void(Description &)> body) {
  Context context(*this, name, body);
  context.before_eaches = this->before_eaches;
  context.after_eaches = this->after_eaches;
  return context.run();
}

bool Description::it(std::string name, std::function<void(ItD &)> body) {
  ItD it(*this, name, body);
  bool result = it.run();
  exec_after_eaches();
  exec_before_eaches();
  return result;
}

bool Description::it(std::function<void(ItD &)> body) {
  ItD it(*this, body);
  bool result = it.run();
  exec_after_eaches();
  exec_before_eaches();
  return result;
}

// template <class U>
// ClassDescription<U> Description::subject(U subject) {
//  return ClassDescription<U>(*this, subject);
//}
//
// template <class U>
// ClassDescription<U> Description::subject(U &subject) {
//  return ClassDescription<U>(*this, subject);
//}
//
// template <class U>
// ClassDescription<std::vector<U>> Description::subject(
//    std::initializer_list<U> init_list) {
//  return ClassDescription<std::vector<U>>(*this, std::vector<U>(init_list));
//}

void Description::before_each(rule_block_t b) {
  before_eaches.push_back(b);

  // Due to how lambdas and their contexts are passed around, we need to prime
  // the environment by executing the before_each, so that when an 'it'
  // declaration's lambda captures that env, it has the correct values for the
  // variables. Truthfully, 'before_each' is a misnomer, as they are not
  // getting executed directly before the lambda's execution as one might
  // expect, but instead before the *next* lambda is declared.
  b();
}

void Description::before_all(rule_block_t b) { b(); }

void Description::after_each(rule_block_t b) { after_eaches.push_back(b); }

void Description::after_all(rule_block_t b) { after_alls.push_back(b); }

void Description::exec_before_eaches() {
  for (rule_block_t b : before_eaches) b();
}

void Description::exec_after_eaches() {
  for (rule_block_t b : after_eaches) b();
}

void Description::let(std::string name, Runnable &body) {
  lets.insert({name, &body});
}

bool Description::run() {
  std::cout << padding() << descr << std::endl;
  body(*this);
  for (auto a : after_alls) a();
  std::cout << std::endl;
  return this->get_status();
}

void Description::reset_lets() {
  for (auto &pair : lets) {
    auto base = static_cast<LetBase *>(pair.second);  // downcast.
    base->reset();
  }

  if (has_parent()) {
    auto parent = static_cast<Description *>(get_parent());
    parent->reset_lets();
  }
}

Runnable *Description::find_let(std::string name) {
  auto got = lets.find(name);

  if (got == lets.end()) {
    if (this->has_parent()) {
      auto parent = static_cast<Description *>(get_parent());
      return parent->find_let(name);
    } else {  // this should never, *ever* happen.
      throw(std::out_of_range("Could not find let '" + name + "'"));
    }
  } else {
    return got->second;
  }
}

template <class T>
Expectations::Expectation<T> ItExpBase::expect(Let<T> let) {
  auto parent = static_cast<Description *>(this->get_parent());
  Let<T> *actual_let = static_cast<Let<T> *>(parent->find_let(let.get_name()));
  T res = actual_let->get_result();
  Expectations::Expectation<T> expectation(*this, res);
  return expectation;
}

bool ItD::run() {
  if (!this->needs_descr()) {
    std::cout << padding() << get_descr() << std::endl;
  }
  body(*this);

  auto parent = static_cast<Description *>(this->get_parent());
  parent->reset_lets();

  return this->get_status();
}

#endif /* DESCRIPTION_H */

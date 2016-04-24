/**
 * @file
 * @brief Defines the Description class and associated functions
 */
#ifndef CPPSPEC_DESCRIPTION_HPP
#define CPPSPEC_DESCRIPTION_HPP
#include <queue>
#include <unordered_map>
#include "it.hpp"

namespace CppSpec {

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
  Result it(std::string descr, std::function<void(ItD &)> body);
  Result it(std::function<void(ItD &)> body);
  Result context(std::string descr, block_t body);

  template <class T>
  Result context(T subject, std::function<void(ClassDescription<T> &)> body);

  //  template <class T>
  //  bool context(T &subject, std::function<void(ClassDescription<T> &)> body);

  template <class T, typename U>
  Result context(std::initializer_list<U> init_list,
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

  template <typename T>
  auto make_let(std::string name, T body) -> Let<decltype(body())>;
  void add_let(std::string name, Runnable &body);
  virtual Result run(Formatters::BaseFormatter &printer) override;
  void reset_lets();
  Runnable *find_let(std::string);
  virtual std::string get_descr() { return descr; }
  virtual const std::string get_descr() const { return descr; }
  virtual std::string get_subject_type() { return ""; }
  virtual const std::string get_subject_type() const { return ""; }
};

typedef Description Context;

Result Description::context(std::string name,
                            std::function<void(Description &)> body) {
  Context context(*this, name, body);
  context.before_eaches = this->before_eaches;
  context.after_eaches = this->after_eaches;
  return context.run(this->get_formatter());
}

Result Description::it(std::string name, std::function<void(ItD &)> body) {
  ItD it(*this, name, body);
  Result result = it.run(this->get_formatter());
  exec_after_eaches();
  exec_before_eaches();
  return result;
}

Result Description::it(std::function<void(ItD &)> body) {
  ItD it(*this, body);
  Result result = it.run(this->get_formatter());
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

/**
 * @brief Object generator for Let.
 *
 * @param body the body of the let statement
 *
 * @return a new Let object
 */
template <typename T>
auto Description::make_let(std::string name, T body) -> Let<decltype(body())> {
  return Let<decltype(body())>(*this, name, body);
}

void Description::add_let(std::string name, Runnable &body) {
  lets.insert({name, &body});
}

Result Description::run(Formatters::BaseFormatter &printer) {
  if (not this->has_formatter()) this->set_printer(printer);
  printer.format(*this);
  body(*this);
  for (auto a : after_alls) a();
  if (this->get_parent() == nullptr) printer.flush();
  return this->get_status() ? Result::success : Result::failure;
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

Result ItD::run(Formatters::BaseFormatter &printer) {
//  if (!this->needs_descr() && printer.mode == BaseFormatter::Mode::verbose) {
//    printer.format(*this);
//  }

  body(*this);

  printer.format(*this);

  auto parent = static_cast<Description *>(this->get_parent());
  parent->reset_lets();

  return this->get_status() ? Result::success : Result::failure;
}

} // ::CppSpec
#endif // CPPSPEC_DESCRIPTION_HPP

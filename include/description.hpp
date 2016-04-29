/**
 * @file
 * @brief Defines the Description class and associated functions
 */
#ifndef CPPSPEC_DESCRIPTION_HPP
#define CPPSPEC_DESCRIPTION_HPP
#pragma once

#include <string>
#include <deque>
#include <queue>
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
  std::unordered_set<LetBase *> lets;

  Description() {}
  explicit Description(std::string descr) noexcept : descr(descr) {}
  Description(Child &parent, std::string descr, block_t body)
      : Runnable(parent), body(body), descr(descr) {}

 public:
  // Constructor
  Description(std::string descr, block_t body) noexcept : body(body),
                                                          descr(descr) {}

  const bool has_subject = false;
  std::deque<rule_block_t> after_alls;
  std::deque<rule_block_t> before_eaches;
  std::deque<rule_block_t> after_eaches;

  // Spec functions
  Result it(std::string descr, std::function<void(ItD &)> body);
  Result it(std::function<void(ItD &)> body);
  Result context(std::string descr, block_t body);

  template <class T>
  Result context(T subject, std::function<void(ClassDescription<T> &)> body);

  template <class T>
  Result context(std::string descr, T subject,
                 std::function<void(ClassDescription<T> &)> body);

  template <class T, typename U>
  Result context(std::initializer_list<U> init_list,
                 std::function<void(ClassDescription<T> &)> body);

  void before_each(rule_block_t block);
  void before_all(rule_block_t block);
  void after_each(rule_block_t block);
  void after_all(rule_block_t block);
  void exec_before_eaches();
  void exec_after_eaches();

  template <typename T>
  auto let(T body) -> Let<decltype(body())>;
  void reset_lets();

  Result run(Formatters::BaseFormatter &printer) override;

  virtual std::string get_descr() { return descr; }
  virtual const std::string get_descr() const { return descr; }
  virtual std::string get_subject_type() { return ""; }
  virtual const std::string get_subject_type() const { return ""; }
};

typedef Description Context;

inline Result Description::context(std::string name,
                                   std::function<void(Description &)> body) {
  Context context(*this, name, body);
  context.before_eaches = this->before_eaches;
  context.after_eaches = this->after_eaches;
  return context.run(this->get_formatter());
}

inline Result Description::it(std::string name,
                              std::function<void(ItD &)> body) {
  ItD it(*this, name, body);
  Result result = it.run(this->get_formatter());
  exec_after_eaches();
  exec_before_eaches();
  return result;
}

inline Result Description::it(std::function<void(ItD &)> body) {
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

inline void Description::before_each(rule_block_t b) {
  before_eaches.push_back(b);

  // Due to how lambdas and their contexts are passed around, we need to prime
  // the environment by executing the before_each, so that when an 'it'
  // declaration's lambda captures that env, it has the correct values for the
  // variables. Truthfully, 'before_each' is a misnomer, as they are not
  // getting executed directly before the lambda's execution as one might
  // expect, but instead before the *next* lambda is declared.
  b();
}

inline void Description::before_all(rule_block_t b) { b(); }

inline void Description::after_each(rule_block_t b) {
  after_eaches.push_back(b);
}

inline void Description::after_all(rule_block_t b) { after_alls.push_back(b); }

inline void Description::exec_before_eaches() {
  for (rule_block_t b : before_eaches) b();
}

inline void Description::exec_after_eaches() {
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
inline auto Description::let(T body) -> Let<decltype(body())> {
  Let<decltype(body())> let(body);
  lets.insert(&let);
  return let;
}

inline Result Description::run(Formatters::BaseFormatter &printer) {
  if (not this->has_formatter()) this->set_printer(printer);
  printer.format(*this);
  body(*this);
  for (auto a : after_alls) a();
  if (this->get_parent() == nullptr) printer.flush();
  return this->get_status() ? Result::success() : Result::failure();
}

inline void Description::reset_lets() {
  for (auto &let : lets) let->reset();
  if (this->has_parent()) this->get_parent_as<Description *>()->reset_lets();
}

inline Result ItD::run(Formatters::BaseFormatter &printer) {
  body(*this);
  printer.format(*this);
  this->get_parent_as<Description *>()->reset_lets();
  return this->get_status() ? Result::success() : Result::failure();
}

}  // namespace CppSpec
#endif  // CPPSPEC_DESCRIPTION_HPP

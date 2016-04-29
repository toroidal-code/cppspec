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
#include "class_description.hpp"

namespace CppSpec {

template <typename T>
class ClassDescription;  // forward-declaration for ClassDescription

class Description : public Runnable {
 public:
  using Block = std::function<void(Description &)>;
  using VoidBlock = std::function<void()>;

 private:
  Block block;

 protected:
  std::string description = "";

  Description() {}

  explicit Description(std::string description) noexcept
      : description(description) {}

  Description(const Child &parent, std::string description,
              Block block) noexcept : Runnable(parent),
                                      block(block),
                                      description(description) {}

  void exec_before_eaches();
  void exec_after_eaches();

 public:
  // Constructor
  Description(std::string description, Block block) noexcept
      : block(block),
        description(description) {}

  const bool has_subject = false;
  std::unordered_set<LetBase *> lets;
  std::deque<VoidBlock> after_alls;
  std::deque<VoidBlock> before_eaches;
  std::deque<VoidBlock> after_eaches;

  // Specify/It functions
  Result it(std::string description, ItD::Block body);
  Result it(ItD::Block body);

  // Context functions
  Result context(std::string name, Block body);

  template <class T>
  Result context(T subject, std::function<void(ClassDescription<T> &)> block);

  template <class T>
  Result context(std::string description, T subject,
                 std::function<void(ClassDescription<T> &)> block);

  template <class T, typename U>
  Result context(std::initializer_list<U> init_list,
                 std::function<void(ClassDescription<T> &)> block);

  void before_each(VoidBlock block);
  void before_all(VoidBlock block);
  void after_each(VoidBlock block);
  void after_all(VoidBlock block);

  template <typename T>
  auto let(T body) -> Let<decltype(body())>;
  void reset_lets() noexcept;

  Result run(Formatters::BaseFormatter &printer) override;

  virtual std::string get_description() noexcept { return description; }
  virtual const std::string get_description() const noexcept {
    return description;
  }

  virtual std::string get_subject_type() noexcept { return ""; }
  virtual const std::string get_subject_type() const noexcept { return ""; }
};

using Context = Description;

inline Result Description::context(std::string description, Block body) {
  Context context(*this, description, body);
  context.before_eaches = this->before_eaches;
  context.after_eaches = this->after_eaches;
  return context.run(this->get_formatter());
}

inline Result Description::it(std::string description, ItD::Block block) {
  ItD it(*this, description, block);
  Result result = it.run(this->get_formatter());
  exec_after_eaches();
  exec_before_eaches();
  return result;
}

inline Result Description::it(ItD::Block block) {
  ItD it(*this, block);
  Result result = it.run(this->get_formatter());
  exec_after_eaches();
  exec_before_eaches();
  return result;
}

inline void Description::before_each(VoidBlock b) {
  before_eaches.push_back(b);

  // Due to how lambdas and their contexts are passed around, we need to prime
  // the environment by executing the before_each, so that when an 'it'
  // declaration's lambda captures that env, it has the correct values for the
  // variables. Truthfully, 'before_each' is a misnomer, as they are not
  // getting executed directly before the lambda's execution as one might
  // expect, but instead before the *next* lambda is declared.
  b();
}

inline void Description::before_all(VoidBlock b) { b(); }

inline void Description::after_each(VoidBlock b) { after_eaches.push_back(b); }

inline void Description::after_all(VoidBlock b) { after_alls.push_back(b); }

inline void Description::exec_before_eaches() {
  for (VoidBlock b : before_eaches) b();
}

inline void Description::exec_after_eaches() {
  for (VoidBlock b : after_eaches) b();
}

/**
 * @brief Object generator for Let.
 *
 * @param body the body of the let statement
 *
 * @return a new Let object
 */
template <typename T>
inline auto Description::let(T block) -> Let<decltype(block())> {
  Let<decltype(block())> let(block);
  lets.insert(&let);
  return let;
}

inline Result Description::run(Formatters::BaseFormatter &printer) {
  if (not this->has_formatter()) this->set_printer(printer);
  printer.format(*this);
  block(*this);
  for (auto a : after_alls) a();
  if (this->get_parent() == nullptr) printer.flush();
  return this->get_status() ? Result::success() : Result::failure();
}

inline void Description::reset_lets() noexcept {
  for (auto &let : lets) let->reset();
  if (this->has_parent()) this->get_parent_as<Description *>()->reset_lets();
}

inline Result ItD::run(Formatters::BaseFormatter &printer) {
  block(*this);
  printer.format(*this);
  this->get_parent_as<Description *>()->reset_lets();
  return this->get_status() ? Result::success() : Result::failure();
}

}  // namespace CppSpec
#endif  // CPPSPEC_DESCRIPTION_HPP

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
#include <forward_list>
#include "it.hpp"
#include "class_description.hpp"

namespace CppSpec {

template <typename T>
class ClassDescription;  // forward-declaration for ClassDescription

class Description : public Runnable {
  using VoidBlock = std::function<void()>;

 public:
  using Block = std::function<void(Description &)>;

  const bool has_subject = false;
  std::forward_list<LetBase *> lets;
  std::deque<VoidBlock> after_alls;
  std::deque<VoidBlock> before_eaches;
  std::deque<VoidBlock> after_eaches;

 private:
  Block block;

 protected:
  std::string description = "";

  // These two constructors are the most basic ones,
  // used to create Descriptions with only their description
  // field initialized. They should only be used by subclasses
  // of Description.
  Description() = default;
  explicit Description(std::string description) noexcept
      : description(description) {}

  Description(const Child &parent, std::string description,
              Block block) noexcept : Runnable(parent),
                                      block(block),
                                      description(description) {}

  void exec_before_eaches();
  void exec_after_eaches();

 public:
  // Copy constructor
  Description(const Description &copy) = default;

  // Primary constructor. Entry of all specs.
  Description(std::string description, Block block) noexcept
      : block(block),
        description(description) {}

  /********* Specify/It *********/

  Result it(std::string description, ItD::Block body);
  Result it(ItD::Block body);

  /********* Context ***********/

  template <class T=std::nullptr_t>
  Result context(std::string name, Block body);

  template <class T>
  Result context(T subject, std::function<void(ClassDescription<T> &)> block);

  template <class T>
  Result context(std::string description, T subject,
                 std::function<void(ClassDescription<T> &)> block);

  template <class T, typename U>
  Result context(std::initializer_list<U> init_list,
                 std::function<void(ClassDescription<T> &)> block);

  /********* Each/All *********/

  void before_each(VoidBlock block);
  void before_all(VoidBlock block);
  void after_each(VoidBlock block);
  void after_all(VoidBlock block);

  /********* Let *********/

  template <typename T>
  auto let(T body) -> Let<decltype(body())>;
  void reset_lets() noexcept;

  /********* Standard getters *********/

  virtual std::string get_description() const noexcept { return description; }
  virtual std::string get_subject_type() const noexcept { return ""; }

  /********* Run *********/

  Result run(Formatters::BaseFormatter &printer) override;
};

using Context = Description;

/*>>>>>>>>>>>>>>>>>>>> Description <<<<<<<<<<<<<<<<<<<<<<<<<*/

/*========= Description::it =========*/

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

/*========= Description::context =========*/

template <class T>
inline Result Description::context(std::string description, Block body) {
  Context context(*this, description, body);
  context.before_eaches = this->before_eaches;
  context.after_eaches = this->after_eaches;
  return context.run(this->get_formatter());
}

/*========= Description:: each/alls =========*/

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

/*----------- private -------------*/

inline void Description::exec_before_eaches() {
  for (VoidBlock &b : before_eaches) b();
}

inline void Description::exec_after_eaches() {
  for (VoidBlock &b : after_eaches) b();
}

/*========= Description::let =========*/

/**
 * @brief Object generator for Let.
 *
 * @param body the body of the let statement
 *
 * @return a new Let object
 */
template <typename T>
auto Description::let(T block) -> Let<decltype(block())> {
  // In reality, this gets inlined due to the fact that it's
  // a templated function. Otherwise we wouldn't be able to
  // add the address of the Let, return the Let by value,
  // and still be able to do a valid deference of the Let
  // pointer later on when we needed to reset the Let.

  Let<decltype(block())> let(block);  // Create a Let
  lets.push_front(&let);              // Add it to our list
  return let;                         // Hand it object off
}

// TODO: Should this be protected?
inline void Description::reset_lets() noexcept {
  // For every let in our list, reset it.
  for (auto &let : lets) let->reset();

  // Recursively reset all the lets in the family tree
  if (this->has_parent()) {
    this->get_parent_as<Description *>()->reset_lets();
  }
}

/*========= Description::run =========*/

inline Result Description::run(Formatters::BaseFormatter &formatter) {
  // If there isn't already a formatter in the family tree, set ours.
  if (!this->has_formatter()) this->set_formatter(formatter);

  formatter.format(*this);              // Format our description in some way
  block(*this);                         // Run the block
  for (VoidBlock &a : after_alls) a();  // Run all our after_alls
  if (!this->has_parent()) formatter.flush();  // Inform the printer we're done

  // Return success or failure
  return this->get_status() ? Result::success() : Result::failure();
}

/*>>>>>>>>>>>>>>>>>>>> ItD <<<<<<<<<<<<<<<<<<<<<<<<<*/

/*========= ItD::run =========*/

inline Result ItD::run(Formatters::BaseFormatter &printer) {
  block(*this);
  printer.format(*this);
  this->get_parent_as<Description *>()->reset_lets();
  return this->get_status() ? Result::success() : Result::failure();
}

}  // namespace CppSpec
#endif  // CPPSPEC_DESCRIPTION_HPP

/**
 * @file
 * @brief Defines the Description class and associated functions
 */
#pragma once

#include <deque>
#include <forward_list>
#include <source_location>
#include <string>
#include <utility>

#include "it.hpp"

namespace CppSpec {

template <typename T>
class ClassDescription;  // forward-declaration for ClassDescription

class Description : public Runnable {
  using VoidBlock = std::function<void()>;

 public:
  using Block = std::function<void(Description&)>;

  const bool has_subject = false;
  std::forward_list<LetBase*> lets{};
  std::deque<VoidBlock> after_alls{};
  std::deque<VoidBlock> before_eaches{};
  std::deque<VoidBlock> after_eaches{};

 private:
  Block block;

 protected:
  std::string description;

  Description(std::source_location location, std::string&& description) noexcept
      : Runnable(location), description(std::move(description)) {}

  Description(Runnable& parent, std::source_location location, const char* description, Block block) noexcept
      : Runnable(parent, location), block(block), description(description) {}

  void exec_before_eaches();
  void exec_after_eaches();

 public:
  // Primary constructor. Entry of all specs.
  Description(const char* description,
              Block block,
              std::source_location location = std::source_location::current()) noexcept
      : Runnable(location), block(block), description(description) {
    this->set_location(location);
  }

  /********* Specify/It *********/

  ItD& it(const char* description, ItD::Block body, std::source_location location = std::source_location::current());
  ItD& it(ItD::Block body, std::source_location location = std::source_location::current());

  /********* Context ***********/

  template <class T = std::nullptr_t>
  Description& context(const char* name, Block body, std::source_location location = std::source_location::current());

  template <class T, class B>
    requires(!std::is_same_v<T, const char*>)
  ClassDescription<T>& context(T subject, B block, std::source_location location = std::source_location::current());

  template <class T, class B>
  ClassDescription<T>& context(const char* description,
                               T subject,
                               B block,
                               std::source_location location = std::source_location::current());

  template <class T, typename U>
  ClassDescription<T>& context(std::initializer_list<U> init_list,
                               std::function<void(ClassDescription<T>&)> block,
                               std::source_location location = std::source_location::current());

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

  [[nodiscard]] virtual std::string get_description() const noexcept { return description; }
  [[nodiscard]] virtual std::string get_subject_type() const noexcept { return ""; }

  /********* Run *********/

  void run() override;
  // std::function<int(int, char **)>
  template <typename Formatter>
  inline auto as_main();
};

using Context = Description;

/*>>>>>>>>>>>>>>>>>>>> Description <<<<<<<<<<<<<<<<<<<<<<<<<*/

/*========= Description::it =========*/

inline ItD& Description::it(const char* description, ItD::Block block, std::source_location location) {
  auto* it = new ItD(*this, location, description, block);
  it->timed_run();
  exec_after_eaches();
  exec_before_eaches();
  return *it;
}

inline ItD& Description::it(ItD::Block block, std::source_location location) {
  auto* it = new ItD(*this, location, block);
  it->timed_run();
  exec_after_eaches();
  exec_before_eaches();
  return *it;
}

/*========= Description::context =========*/

template <class T>
inline Context& Description::context(const char* description, Block body, std::source_location location) {
  auto* context = new Context(*this, location, description, body);
  context->before_eaches = this->before_eaches;
  context->after_eaches = this->after_eaches;
  context->timed_run();
  return *context;
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

inline void Description::before_all(VoidBlock b) {
  b();
}

inline void Description::after_each(VoidBlock b) {
  after_eaches.push_back(b);
}

inline void Description::after_all(VoidBlock b) {
  after_alls.push_back(b);
}

/*----------- private -------------*/

inline void Description::exec_before_eaches() {
  for (VoidBlock& b : before_eaches)
    b();
}

inline void Description::exec_after_eaches() {
  for (VoidBlock& b : after_eaches)
    b();
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
  for (auto& let : lets)
    let->reset();

  // Recursively reset all the lets in the family tree
  if (this->has_parent()) {
    this->get_parent_as<Description>()->reset_lets();
  }
}

/*========= Description::run =========*/

inline void Description::run() {
  block(*this);  // Run the block
  for (VoidBlock& a : after_alls)
    a();  // Run all our after_alls
}

/*>>>>>>>>>>>>>>>>>>>> ItD <<<<<<<<<<<<<<<<<<<<<<<<<*/

/*========= ItD::run =========*/

inline void ItD::run() {
  block(*this);
  this->get_parent_as<Description>()->reset_lets();
}

}  // namespace CppSpec

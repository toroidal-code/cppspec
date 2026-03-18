/**
 * @file
 * @brief Defines the Description class and associated functions
 */
#pragma once

#include <deque>
#include <forward_list>
#include <list>
#include <memory>
#include <source_location>
#include <string>
#include <utility>

#include "it.hpp"

namespace CppSpec {

template <class T>
class ClassDescription;  // forward-declaration for ClassDescription

class Description : public Runnable {
  using VoidBlock = std::function<void()>;

 public:
  using Block = std::function<void(Description&)>;

  std::forward_list<LetBase*> lets;
  std::deque<VoidBlock> after_alls;
  std::deque<VoidBlock> before_eaches;
  std::deque<VoidBlock> after_eaches;

 private:
  Block block;
  std::list<std::unique_ptr<LetBase>> owned_lets_;

 protected:
  std::string description;

  void exec_before_eaches();
  void exec_after_eaches();

 public:
  // Primary constructor. Entry of all specs.
  Description(const char* description,
              Block block,
              std::source_location location = std::source_location::current()) noexcept
      : Runnable(location), block(std::move(block)), description(description) {
    this->set_location(location);
  }

  Description(std::source_location location, std::string&& description) noexcept
      : Runnable(location), description(std::move(description)) {}

  Description(std::source_location location, const char* description, Block block) noexcept
      : Runnable(location), block(std::move(block)), description(description) {}

  /********* Specify/It *********/

  ItD& it(const char* name, ItD::Block body, std::source_location location = std::source_location::current());
  ItD& it(ItD::Block body, std::source_location location = std::source_location::current());

  /********* Context ***********/

  template <class T = std::nullptr_t>
  Description& context(const char* description,
                       Block body,
                       std::source_location location = std::source_location::current());

  template <Util::not_c_string T, class B>
  ClassDescription<T>& context(T& subject, B block, std::source_location location = std::source_location::current());

  template <class T, class B>
  ClassDescription<T>& context(const char* description,
                               T& subject,
                               B block,
                               std::source_location location = std::source_location::current());

  template <Util::not_c_string T, class B>
  ClassDescription<T>& context(T&& subject, B block, std::source_location location = std::source_location::current());

  template <class T, class B>
  ClassDescription<T>& context(const char* description,
                               T&& subject,
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

  template <typename F>
  auto& let(F factory);
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
  exec_before_eaches();
  auto* it = this->make_child<ItD>(location, description, block);
  it->timed_run();
  exec_after_eaches();
  return *it;
}

inline ItD& Description::it(ItD::Block block, std::source_location location) {
  exec_before_eaches();
  auto* it = this->make_child<ItD>(location, block);
  it->timed_run();
  exec_after_eaches();
  return *it;
}

/*========= Description::context =========*/

template <class T>
inline Context& Description::context(const char* description, Block body, std::source_location location) {
  auto* context = this->make_child<Context>(location, description, body);
  context->before_eaches = this->before_eaches;
  context->after_eaches = this->after_eaches;
  context->timed_run();
  return *context;
}

/*========= Description:: each/alls =========*/

inline void Description::before_each(VoidBlock b) {
  before_eaches.push_back(std::move(b));
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
  for (VoidBlock& b : before_eaches) {
    b();
  }
}

inline void Description::exec_after_eaches() {
  for (VoidBlock& b : after_eaches) {
    b();
  }
}

/*========= Description::let =========*/

template <typename F>
auto& Description::let(F factory) {
  using T = decltype(std::declval<F>()());
  auto ptr = std::make_unique<Let<T>>(std::move(factory));
  auto* raw = ptr.get();
  owned_lets_.push_back(std::move(ptr));
  lets.push_front(raw);
  return *raw;
}

inline void Description::reset_lets() noexcept {
  // For every let in our list, reset it.
  for (auto& let : lets) {
    let->reset();
  }

  // Recursively reset all the lets in the family tree
  if (this->has_parent()) {
    this->get_parent_as<Description>()->reset_lets();
  }
}

/*========= Description::run =========*/

inline void Description::run() {
  block(*this);  // Run the block
  for (VoidBlock& a : after_alls) {
    a();  // Run all our after_alls
  }
}

/*>>>>>>>>>>>>>>>>>>>> ItD <<<<<<<<<<<<<<<<<<<<<<<<<*/

/*========= ItD::run =========*/

inline void ItD::run() {
  block(*this);
  this->get_parent_as<Description>()->reset_lets();
}

}  // namespace CppSpec

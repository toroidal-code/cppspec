/** @file */
#pragma once
#include <source_location>
#include <string>

#include "description.hpp"

namespace CppSpec {

/**
 * @brief A Description with a defined subject
 *
 * A ClassDescription is a subclass of Description that
 * allows for templating and specification of the subject
 * of the tests prior to any `it` objects.
 *
 * It is also aliased to the context keyword through
 * Description whenever it is templated, allowing the
 * implicit subject in any child `it` blocks
 */
template <class T>
class ClassDescription : public Description {
  using Block = std::function<void(ClassDescription<T>&)>;

  Block block;
  std::string type;

 public:
  T subject;  // subject field public for usage in `expect([self.]subject)`

  // Constructor
  // if there's no explicit subject given, then use
  // the default constructor of the given type as the implicit subject.
  ClassDescription(Block block, std::source_location location = std::source_location::current())
      : Description(location, Pretty::to_word(subject)),
        block(block),
        type(" : " + Util::demangle(typeid(T).name())),
        subject(T()) {}

  ClassDescription(const char* description,
                   Block block,
                   std::source_location location = std::source_location::current())
      : Description(location, description), block(block), subject(T()) {}

  ClassDescription(const char* description,
                   T& subject,
                   Block block,
                   std::source_location location = std::source_location::current())
      : Description(location, description), block(block), subject(subject) {}

  template <Util::not_c_string U>
  ClassDescription(U& subject, Block block, std::source_location location = std::source_location::current())
      : Description(location, Pretty::to_word(subject)),
        block(block),
        type(" : " + Util::demangle(typeid(T).name())),
        subject(subject) {}

  ClassDescription(const char* description,
                   T&& subject,
                   Block block,
                   std::source_location location = std::source_location::current())
      : Description(location, description), block(block), subject(std::move(subject)) {}

  template <Util::not_c_string U>
  ClassDescription(U&& subject, Block block, std::source_location location = std::source_location::current())
      : Description(location, Pretty::to_word(subject)),
        block(block),
        type(" : " + Util::demangle(typeid(T).name())),
        subject(std::forward<U>(subject)) {}

  template <typename U>
  ClassDescription(std::initializer_list<U> init_list,
                   Block block,
                   std::source_location location = std::source_location::current())
      : Description(location, Pretty::to_word(subject)),
        block(block),
        type(" : " + Util::demangle(typeid(T).name())),
        subject(T(init_list)) {}

  template <typename U>
  ClassDescription(const char* description,
                   std::initializer_list<U> init_list,
                   Block block,
                   std::source_location location = std::source_location::current())
      : Description(location, description), block(block), subject(T(init_list)) {}

  ItCD<T>& it(const char* name,
              std::function<void(ItCD<T>&)> block,
              std::source_location location = std::source_location::current());
  ItCD<T>& it(std::function<void(ItCD<T>&)> block, std::source_location location = std::source_location::current());

  template <class U = std::nullptr_t, class B>
  ClassDescription<T>& context(const char* description,
                               B block,
                               std::source_location location = std::source_location::current());

  template <class U, class B>
  ClassDescription<U>& context(const char* description,
                               U& subject,
                               B block,
                               std::source_location location = std::source_location::current());

  template <class U, class B>
  ClassDescription<U>& context(U& subject, B block, std::source_location location = std::source_location::current()) {
    return this->context("", subject, block, location);
  }

  template <class U, class B>
  ClassDescription<U>& context(const char* description,
                               U&& subject,
                               B block,
                               std::source_location location = std::source_location::current());

  template <class U, class B>
  ClassDescription<U>& context(U&& subject, B block, std::source_location location = std::source_location::current()) {
    return this->context("", std::forward<U>(subject), block, location);
  }
  void run() override;

  [[nodiscard]] std::string get_subject_type() const noexcept override { return type; }
};

template <Util::not_c_string U>
ClassDescription(U&, std::function<void(ClassDescription<U>&)>, std::source_location) -> ClassDescription<U>;

template <Util::not_c_string U>
ClassDescription(U&&, std::function<void(ClassDescription<U>&)>, std::source_location) -> ClassDescription<U>;

template <class T>
using ClassContext = ClassDescription<T>;

template <class T>
template <class U, class B>
ClassContext<U>& ClassDescription<T>::context(const char* description,
                                              U& subject,
                                              B block,
                                              std::source_location location) {
  auto* context = this->make_child<ClassContext<U>>(description, subject, block, location);
  context->ClassContext<U>::before_eaches = this->before_eaches;
  context->ClassContext<U>::after_eaches = this->after_eaches;
  context->timed_run();
  return *context;
}

template <class T>
template <class U, class B>
ClassContext<U>& ClassDescription<T>::context(const char* description,
                                              U&& subject,
                                              B block,
                                              std::source_location location) {
  auto* context = this->make_child<ClassContext<U>>(description, std::forward<U>(subject), block, location);
  context->ClassContext<U>::before_eaches = this->before_eaches;
  context->ClassContext<U>::after_eaches = this->after_eaches;
  context->timed_run();
  return *context;
}

template <class T>
template <class U, class B>
ClassContext<T>& ClassDescription<T>::context(const char* description, B block, std::source_location location) {
  auto* context = this->make_child<ClassContext<T>>(description, this->subject, block, location);
  context->before_eaches = this->before_eaches;
  context->after_eaches = this->after_eaches;
  context->timed_run();
  return *context;
}

template <Util::not_c_string T, class B>
ClassContext<T>& Description::context(T& subject, B block, std::source_location location) {
  auto* context = this->make_child<ClassContext<T>>(subject, block, location);
  context->before_eaches = this->before_eaches;
  context->after_eaches = this->after_eaches;
  context->timed_run();
  return *context;
}

template <class T, class B>
ClassContext<T>& Description::context(const char* description, T& subject, B block, std::source_location location) {
  auto* context = this->make_child<ClassContext<T>>(description, subject, block, location);
  context->before_eaches = this->before_eaches;
  context->after_eaches = this->after_eaches;
  context->timed_run();
  return *context;
}

template <Util::not_c_string T, class B>
ClassContext<T>& Description::context(T&& subject, B block, std::source_location location) {
  auto* context = this->make_child<ClassContext<T>>(std::forward<T>(subject), block, location);
  context->before_eaches = this->before_eaches;
  context->after_eaches = this->after_eaches;
  context->timed_run();
  return *context;
}

template <class T, class B>
ClassContext<T>& Description::context(const char* description, T&& subject, B block, std::source_location location) {
  auto* context = this->make_child<ClassContext<T>>(description, std::forward<T>(subject), block, location);
  context->before_eaches = this->before_eaches;
  context->after_eaches = this->after_eaches;
  context->timed_run();
  return *context;
}

template <class T, typename U>
ClassContext<T>& Description::context(std::initializer_list<U> init_list,
                                      std::function<void(ClassDescription<T>&)> block,
                                      std::source_location location) {
  auto* context = this->make_child<ClassContext<T>>(T(init_list), block, location);
  context->before_eaches = this->before_eaches;
  context->after_eaches = this->after_eaches;
  context->timed_run();
  return *context;
}

/**
 * Jasmine-style `it` declaration, with an explicit docstring
 * provided for verbose printing.
 *
 * As this is an ItCD, it passes along associated type information
 * about the implicit subject from the containing
 * ClassDescription / ClassContext.
 *
 * @code
 *   describe_a <std::string> (_{
 *     it("is empty upon initialization", _{
 *       is_expected.to_be_empty();
 *     });
 *   });
 * @endcode
 *
 * @param name the name of the test
 * @param block the contents of test
 *
 * @return the result of the test
 */
template <class T>
ItCD<T>& ClassDescription<T>::it(const char* name, std::function<void(ItCD<T>&)> block, std::source_location location) {
  auto* it = this->make_child<ItCD<T>>(location, this->subject, name, block);
  it->timed_run();
  exec_after_eaches();
  exec_before_eaches();
  return *it;
}

/**
 * Rspec-style `it` declaration, with an implicit docstring
 * generated by the contained `expect`s. Particularly handy
 * if testing multiple facets of a single behavior and there
 * is a desire to be verbose, as each expectation prints its
 * own docstring.
 *
 * As this is an ItCD, it passes along associated type information
 * about the implicit subject from the containing
 * ClassDescription / ClassContext.
 *
 * @code
 *   describe_a <std::string> ("", _{
 *     it(_{ is_expected.to_be_empty(); });
 *   });
 * @endcode
 *
 * @param block the contents of the test
 *
 * @return the result of the test
 */
template <class T>
ItCD<T>& ClassDescription<T>::it(std::function<void(ItCD<T>&)> block, std::source_location location) {
  auto* it = this->make_child<ItCD<T>>(location, this->subject, block);
  it->timed_run();
  exec_after_eaches();
  exec_before_eaches();
  return *it;
}

template <class T>
void ClassDescription<T>::run() {
  this->block(*this);
  for (const auto& a : after_alls) {
    a();
  }
}

template <class T>
void ItCD<T>::run() {
  this->block(*this);
  auto* cd = this->get_parent_as<ClassDescription<T>>();
  cd->reset_lets();
}

}  // namespace CppSpec

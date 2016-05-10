/** @file */
#ifndef CPPSPEC_CLASS_DESCRIPTION_HPP
#define CPPSPEC_CLASS_DESCRIPTION_HPP
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
  using Block = std::function<void(ClassDescription<T> &)>;

  Block block;
  std::string type = "";

 public:
  const bool has_subject = true;
  T subject;  // subject field public for usage in `expect([self.]subject)`

  // Constructor
  // if there's no explicit subject given, then use
  // the default constructor of the given type as the implicit subject.
  ClassDescription<T>(Block block)
      : Description(),
        block(block),
        type(" : " + Util::demangle(typeid(T).name())),
        subject(T()) {
    this->description = Pretty::to_word(subject);
  }

  ClassDescription<T>(std::string description, Block block)
      : Description(description), block(block), subject(T()) {}

  ClassDescription(T subject, Block block)
      : Description(Pretty::to_word(subject)),
        block(block),
        type(" : " + Util::demangle(typeid(T).name())),
        subject(subject) {}

  ClassDescription(std::string description, T subject, Block block)
      : Description(description), block(block), subject(subject) {}

  ClassDescription(T &subject, Block block)
      : Description(Pretty::to_word(subject)),
        block(block),
        type(" : " + Util::demangle(typeid(T).name())),
        subject(subject) {}

  template <typename U>
  ClassDescription(std::initializer_list<U> init_list, Block block)
      : block(block),
        type(" : " + Util::demangle(typeid(T).name())),
        subject(T(init_list)) {
    this->description = Pretty::to_word(subject);
  }

  template <typename U>
  ClassDescription(std::string description, std::initializer_list<U> init_list,
                   Block block)
      : Description(description), block(block), subject(T(init_list)) {}

  Result it(std::string description, std::function<void(ItCD<T> &)> block);
  Result it(std::function<void(ItCD<T> &)> block);
  /** @brief an alias for it */
  Result specify(std::string description,
                 std::function<void(ItCD<T> &)> block) {
    return it(description, block);
  }
  /** @brief an alias for it */
  Result specify(std::function<void(ItCD<T> &)> block) { return it(block); }

  template <class U>
  Result context(std::string description, U subject,
                 std::function<void(ClassDescription<U> &)> block);
  template <class U>
  Result context(std::string description, U &subject,
                 std::function<void(ClassDescription<U> &)> block);
  template <class U>
  Result context(U subject, std::function<void(ClassDescription<U> &)> block);
  template <class U>
  Result context(U &subject, std::function<void(ClassDescription<U> &)> block);

  Result context(std::string description,
                 std::function<void(ClassDescription<T> &)> block);

  Result run(Formatters::BaseFormatter &printer) override;

  std::string get_subject_type() const noexcept override { return type; }
};

template <class T>
using ClassContext = ClassDescription<T>;

template <class T>
template <class U>
Result ClassDescription<T>::context(
    std::string description, U subject,
    std::function<void(ClassDescription<U> &)> block) {
  ClassContext<U> context(description, subject, block);
  context.set_parent(this);
  context.ClassContext<U>::before_eaches = this->before_eaches;
  context.ClassContext<U>::after_eaches = this->after_eaches;
  return context.run(this->get_formatter());
}

template <class T>
template <class U>
Result ClassDescription<T>::context(
    U subject, std::function<void(ClassDescription<U> &)> block) {
  return this->context("", std::forward<U>(subject), block);
}

template <class T>
template <class U>
Result ClassDescription<T>::context(
    std::string description, U &subject,
    std::function<void(ClassDescription<U> &)> block) {
  ClassContext<U> context(description, subject, block);
  context.set_parent(this);
  context.ClassContext<U>::before_eaches = this->before_eaches;
  context.ClassContext<U>::after_eaches = this->after_eaches;
  return context.run(this->get_formatter());
}

template <class T>
template <class U>
Result ClassDescription<T>::context(
    U &subject, std::function<void(ClassDescription<U> &)> block) {
  return this->context("", std::forward<U>(subject), block);
}

template <class T>
Result ClassDescription<T>::context(
    std::string description, std::function<void(ClassDescription<T> &)> block) {
  ClassContext<T> context(description, this->subject, block);
  context.set_parent(this);
  context.before_eaches = this->before_eaches;
  context.after_eaches = this->after_eaches;
  return context.run(this->get_formatter());
}

template <class T>
Result Description::context(T subject,
                            std::function<void(ClassDescription<T> &)> block) {
  return this->context("", subject, block);
}

template <class T>
Result Description::context(std::string description, T subject,
                            std::function<void(ClassDescription<T> &)> block) {
  ClassContext<T> context(description, subject, block);
  context.set_parent(this);
  context.before_eaches = this->before_eaches;
  context.after_eaches = this->after_eaches;
  return context.run(this->get_formatter());
}

template <class T, typename U>
Result Description::context(std::initializer_list<U> init_list,
                            std::function<void(ClassDescription<T> &)> block) {
  ClassContext<T> context(T(init_list), block);
  context.set_parent(this);
  context.before_eaches = this->before_eaches;
  context.after_eaches = this->after_eaches;
  return context.run(this->get_formatter());
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
Result ClassDescription<T>::it(std::string name,
                               std::function<void(ItCD<T> &)> block) {
  ItCD<T> it(*this, this->subject, name, block);
  Result result = it.run(this->get_formatter());
  exec_after_eaches();
  exec_before_eaches();
  return result;
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
Result ClassDescription<T>::it(std::function<void(ItCD<T> &)> block) {
  ItCD<T> it(*this, this->subject, block);
  Result result = it.run(this->get_formatter());
  exec_after_eaches();
  exec_before_eaches();
  return result;
}

template <class T>
Result ClassDescription<T>::run(Formatters::BaseFormatter &printer) {
  if (not this->has_formatter()) this->set_formatter(printer);
  printer.format(*this);
  this->block(*this);
  for (const auto &a : after_alls) a();
  if (this->get_parent() == nullptr) printer.flush();
  return this->get_status() ? Result::success() : Result::failure();
}

template <class T>
ExpectationValue<T> ItCD<T>::is_expected() {
  auto cd = static_cast<ClassDescription<T> *>(this->get_parent());
  ExpectationValue<T> expectation(*this, cd->subject);
  return expectation;
}

template <class T>
Result ItCD<T>::run(Formatters::BaseFormatter &printer) {
  this->block(*this);
  printer.format(*this);
  auto cd = static_cast<ClassDescription<T> *>(this->get_parent());
  cd->reset_lets();
  return this->get_status() ? Result::success() : Result::failure();
}

}  // namespace CppSpec
#endif  // CPPSPEC_CLASS_DESCRIPTION_HPP

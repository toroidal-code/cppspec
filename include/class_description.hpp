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
  typedef std::function<void(ClassDescription<T> &)> block_t;
  block_t body;
  bool first;
  std::string type = "";

 public:
  T subject;  // subject field public for usage in `expect([self.]subject)`

  // Constructor
  // if there's no explicit subject given, then use
  // the default constructor of the given type as the implicit subject.
  ClassDescription<T>(block_t body)
      : Description(),
        body(body),
        type(" : " + Util::demangle(typeid(T).name())),
        subject(T()) {
    this->descr = Pretty::to_word(subject);
  }

  ClassDescription<T>(std::string descr, block_t body)
      : Description(descr), body(body), subject(T()) {}

  ClassDescription(T subject, block_t body)
      : Description(Pretty::to_word(subject)),
        body(body),
        type(" : " + Util::demangle(typeid(T).name())),
        subject(subject) {}

  ClassDescription(std::string descr, T subject, block_t body)
      : Description(descr), body(body), subject(subject) {}

  ClassDescription(T &subject, block_t body)
      : Description(Pretty::to_word(subject)),
        body(body),
        type(" : " + Util::demangle(typeid(T).name())),
        subject(subject) {}

//  ClassDescription(std::string descr, T &subject, block_t body)
//      : Description(descr), body(body), subject(subject) {}

  template <typename U>
  ClassDescription(std::initializer_list<U> init_list, block_t body)
      : body(body),
        type(" : " + Util::demangle(typeid(T).name())),
        subject(T(init_list)) {
    this->descr = Pretty::to_word(subject);
  }

  template <typename U>
  ClassDescription(std::string descr, std::initializer_list<U> init_list,
                   block_t body)
      : Description(descr), body(body), subject(T(init_list)) {}

  ClassDescription<T>(Description &d) : Description(d) {}

  const bool has_subject = true;

  Result it(std::string descr, std::function<void(ItCd<T> &)> body);
  Result it(std::function<void(ItCd<T> &)> body);
  /** @brief an alias for it */
  Result specify(std::string descr, std::function<void(ItCd<T> &)> body) {
    return it(descr, body);
  }
  /** @brief an alias for it */
  Result specify(std::function<void(ItCd<T> &)> body) { return it(body); }

  template <class U>
  Result context(std::string descr, U subject,
                 std::function<void(ClassDescription<U> &)> body);
  template <class U>
  Result context(std::string descr, U &subject,
                 std::function<void(ClassDescription<U> &)> body);
  template <class U>
  Result context(U subject, std::function<void(ClassDescription<U> &)> body);
  template <class U>
  Result context(U &subject, std::function<void(ClassDescription<U> &)> body);

  Result context(std::string descr, std::function<void(ClassDescription<T> &)> body);

  Result run(Formatters::BaseFormatter &printer) override;

  std::string get_descr() override { return descr; }
  const std::string get_descr() const override { return descr; }
  std::string get_subject_type() override { return type; }
  const std::string get_subject_type() const override { return type; }
};

template <class T>
using ClassContext = ClassDescription<T>;

template <class T>
template <class U>
Result ClassDescription<T>::context(
    std::string descr, U subject,
    std::function<void(ClassDescription<U> &)> body) {
  ClassContext<U> context(descr, subject, body);
  context.set_parent(this);
  context.ClassContext<U>::before_eaches = this->before_eaches;
  context.ClassContext<U>::after_eaches = this->after_eaches;
  return context.run(this->get_formatter());
}

template <class T>
template <class U>
Result ClassDescription<T>::context(
    U subject, std::function<void(ClassDescription<U> &)> body) {
  return context("", std::forward<U>(subject), body);
}

template <class T>
template <class U>
Result ClassDescription<T>::context(
    std::string descr, U &subject,
    std::function<void(ClassDescription<U> &)> body) {
  ClassContext<U> context(descr, subject, body);
  context.set_parent(this);
  context.ClassContext<U>::before_eaches = this->before_eaches;
  context.ClassContext<U>::after_eaches = this->after_eaches;
  return context.run(this->get_formatter());
}

template <class T>
template <class U>
Result ClassDescription<T>::context(
    U &subject, std::function<void(ClassDescription<U> &)> body) {
  return context("", std::forward<U>(subject), body);
}

template <class T>
Result ClassDescription<T>::context(std::string descr,
    std::function<void(ClassDescription<T> &)> body) {
  ClassContext<T> context(descr, this->subject, body);
  context.set_parent(this);
  context.before_eaches = this->before_eaches;
  context.after_eaches = this->after_eaches;
  return context.run(this->get_formatter());
}

template <class T>
Result Description::context(T subject,
                            std::function<void(ClassDescription<T> &)> body) {
  return this->context("", subject, body);
}

template <class T>
Result Description::context(std::string descr, T subject,
                            std::function<void(ClassDescription<T> &)> body) {
  ClassContext<T> context(descr, subject, body);
  context.set_parent(this);
  context.before_eaches = this->before_eaches;
  context.after_eaches = this->after_eaches;
  return context.run(this->get_formatter());
}


// template <class T>
// ClassContext<T>& Description::context(
//     T& subject, std::function<void(ClassDescription<T>&)> body) {
//   return context(subject, body);
// }

template <class T, typename U>
Result Description::context(std::initializer_list<U> init_list,
                            std::function<void(ClassDescription<T> &)> body) {
  ClassContext<T> context(T(init_list), body);
  context.set_parent(this);
  context.before_eaches = this->before_eaches;
  context.after_eaches = this->after_eaches;
  return context.run(this->get_formatter());
}

/**
 * Jasmine-style `it` declaration, with an explicit docstring
 * provided for verbose printing.
 *
 * As this is an ItCd, it passes along associated type information
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
 * @param body the contents of test
 *
 * @return the result of the test
 */
template <class T>
Result ClassDescription<T>::it(std::string name,
                               std::function<void(ItCd<T> &)> body) {
  ItCd<T> it(*this, this->subject, name, body);
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
 * As this is an ItCd, it passes along associated type information
 * about the implicit subject from the containing
 * ClassDescription / ClassContext.
 *
 * @code
 *   describe_a <std::string> ("", _{
 *     it(_{ is_expected.to_be_empty(); });
 *   });
 * @endcode
 *
 * @param body the contents of the test
 *
 * @return the result of the test
 */
template <class T>
Result ClassDescription<T>::it(std::function<void(ItCd<T> &)> body) {
  ItCd<T> it(*this, this->subject, body);
  Result result = it.run(this->get_formatter());
  exec_after_eaches();
  exec_before_eaches();
  return result;
}

template <class T>
Result ClassDescription<T>::run(Formatters::BaseFormatter &printer) {
  if (not this->has_formatter()) this->set_printer(printer);
  printer.format(*this);
  body(*this);
  for (auto a : after_alls) a();
  if (this->get_parent() == nullptr) printer.flush();
  return this->get_status() ? Result::success() : Result::failure();
}

template <class T>
Expectations::ExpectationValue <T> ItCd<T>::is_expected() {
  auto cd = static_cast<ClassDescription<T> *>(this->get_parent());
  Expectations::ExpectationValue<T> expectation(*this, cd->subject);
  return expectation;
}

template <class T>
Result ItCd<T>::run(Formatters::BaseFormatter &printer) {
  //  if (!this->needs_descr() && printer.mode == BaseFormatter::Mode::verbose)
  //  {
  //    printer.format(*this);
  //  }

  body(*this);
  printer.format(*this);

  auto cd = static_cast<ClassDescription<T> *>(this->get_parent());
  cd->reset_lets();
  return this->get_status() ? Result::success() : Result::failure();
}

}  // namespace CppSpec
#endif  // CPPSPEC_CLASS_DESCRIPTION_HPP

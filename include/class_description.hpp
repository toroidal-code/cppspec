#ifndef CPPSPEC_CLASS_DESCRIPTION_HPP
#define CPPSPEC_CLASS_DESCRIPTION_HPP
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
  };

  ClassDescription<T>(std::string descr, block_t body)
      : Description(descr), body(body), subject(T()){};

  ClassDescription(T subject, block_t body)
      : Description(Pretty::to_word(subject)),
        body(body),
        type(" : " + Util::demangle(typeid(T).name())),
        subject(subject){};

  ClassDescription(std::string descr, T subject, block_t body)
      : Description(descr), body(body), subject(subject){};

  ClassDescription(T &subject, block_t body)
      : Description(Pretty::to_word(subject)),
        body(body),
        type(" : " + Util::demangle(typeid(T).name())),
        subject(subject){};

  ClassDescription(std::string descr, T &subject, block_t body)
      : Description(descr), body(body), subject(subject){};

  template <typename U>
  ClassDescription(std::initializer_list<U> init_list, block_t body)
      : body(body),
        type(" : " + Util::demangle(typeid(T).name())),
        subject(T(init_list)) {
    this->descr = Pretty::to_word(subject);
  };

  template <typename U>
  ClassDescription(std::string descr, std::initializer_list<U> init_list,
                   block_t body)
      : Description(descr), body(body), subject(T(init_list)){};

  ClassDescription<T>(Description &d) : Description(d){};

  const bool has_subject = true;

  Result it(std::string descr, std::function<void(ItCd<T> &)> body);
  Result it(std::function<void(ItCd<T> &)> body);
  Result context(T subject, block_t body);
  Result context(T &subject, block_t body);
  Result context(block_t body);
  Result run(BasePrinter &printer) override;
  virtual std::string get_descr() override;
  virtual const std::string get_descr() const override;
};

template <class T>
using ClassContext = ClassDescription<T>;

template <class T>
Result ClassDescription<T>::context(
    T subject, std::function<void(ClassDescription &)> body) {
  ClassContext<T> context(subject, body);
  context.set_parent(this);
  context.before_eaches = this->before_eaches;
  context.after_eaches = this->after_eaches;
  return context.run(this->get_printer());
}

template <class T>
Result ClassDescription<T>::context(
    T &subject, std::function<void(ClassDescription &)> body) {
  return context(subject, body);
}

template <class T>
Result ClassDescription<T>::context(
    std::function<void(ClassDescription &)> body) {
  ClassContext<T> context(body);
  context.set_parent(this);
  context.before_eaches = this->before_eaches;
  context.after_eaches = this->after_eaches;
  return context.run(this->get_printer());
}

template <class T>
Result Description::context(T subject,
                            std::function<void(ClassDescription<T> &)> body) {
  ClassContext<T> context(body);
  context.subject = subject;
  context.set_parent(this);
  context.before_eaches = this->before_eaches;
  context.after_eaches = this->after_eaches;
  return context.run(this->get_printer());
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
  return context.run(this->get_printer());
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
  Result result = it.run(this->get_printer());
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
  Result result = it.run(this->get_printer());
  exec_after_eaches();
  exec_before_eaches();
  return result;
}

template <class T>
Result ClassDescription<T>::run(BasePrinter &printer) {
  if (not this->has_printer()) this->set_printer(printer);
  printer.print(*this);
  body(*this);
  for (auto a : after_alls) a();
  if (this->get_parent() == nullptr) printer.flush();
  return this->get_status() ? Result::success : Result::failure;
}
template <class T>
std::string ClassDescription<T>::get_descr() {
  if (this->get_printer().mode == BasePrinter::Mode::TAP) {
    return descr;
  } else {
    return descr + type;
  }
}

template <class T>
const std::string ClassDescription<T>::get_descr() const {
  if (const_cast<ClassDescription<T> *>(this)->get_printer().mode ==
      BasePrinter::Mode::TAP) {
    return descr;
  } else {
    return descr + type;
  }
}

template <class T>
Expectations::Expectation<T> ItCd<T>::is_expected() {
  auto cd = static_cast<ClassDescription<T> *>(this->get_parent());
  Expectations::Expectation<T> expectation(*this, cd->subject);
  return expectation;
}

template <class T>
Result ItCd<T>::run(BasePrinter &printer) {
  if (!this->needs_descr() && printer.mode == BasePrinter::Mode::verbose) {
    printer.print(*this);
  }

  body(*this);

  if (printer.mode == BasePrinter::Mode::TAP ||
      printer.mode == BasePrinter::Mode::terse) {
    printer.print(*this);
  }

  auto cd = static_cast<ClassDescription<T> *>(this->get_parent());
  cd->reset_lets();
  return this->get_status() ? Result::success : Result::failure;
}

} // ::CppSpec
#endif // CPPSPEC_CLASS_DESCRIPTION_HPP

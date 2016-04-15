#ifndef CLASS_DESCRIPTION_H
#define CLASS_DESCRIPTION_H
#include "description.hpp"

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
  typedef std::function<void(ClassDescription<T>&)> block_t;
  block_t body;
  bool first;

 public:
  T subject;  // subject field exposed for usage in `expect([self.]subject)`

  // Constructor
  // if there's no explicit subject given, then use
  // the default constructor of the given type as the implicit subject.
  ClassDescription<T>(block_t body) : Description(), body(body), subject(T()) {
    this->descr = Pretty::to_word_type(subject);
  };

  ClassDescription<T>(std::string descr, block_t body)
      : Description(descr), body(body), subject(T()){};

  ClassDescription(T subject, block_t body)
      : Description(Pretty::to_word_type(subject)),
        body(body),
        subject(subject){};

  ClassDescription(std::string descr, T subject, block_t body)
      : Description(descr), body(body), subject(subject){};

  ClassDescription(T& subject, block_t body)
      : Description(Pretty::to_word_type(subject)),
        body(body),
        subject(subject){};

  ClassDescription(std::string descr, T& subject, block_t body)
      : Description(descr), body(body), subject(subject){};

  template <typename U>
  ClassDescription(std::initializer_list<U> init_list, block_t body)
      : Description(), body(body), subject(T(init_list)) {
    this->descr = Pretty::to_word_type(subject);
  };

  template <typename U>
  ClassDescription(std::string descr, std::initializer_list<U> init_list,
                   block_t body)
      : Description(descr), body(body), subject(T(init_list)){};

  ClassDescription<T>(Description& d) : Description(d){};

  const bool has_subject = true;

  bool it(std::string descr, std::function<void(ItCd<T>&)> body);
  bool it(std::function<void(ItCd<T>&)> body);
  bool context(T subject, block_t body);
  bool context(T& subject, block_t body);
  bool context(block_t body);
  bool run();
};

template <class T>
using ClassContext = ClassDescription<T>;

template <class T>
bool ClassDescription<T>::context(T subject,
                                  std::function<void(ClassDescription&)> body) {
  ClassContext<T> context(subject, body);
  context.set_parent(this);
  context.before_eaches = this->before_eaches;
  context.after_eaches = this->after_eaches;
  return context.run();
}

template <class T>
bool ClassDescription<T>::context(T& subject,
                                  std::function<void(ClassDescription&)> body) {
  return context(subject, body);
}

template <class T>
bool ClassDescription<T>::context(std::function<void(ClassDescription&)> body) {
  ClassContext<T> context(body);
  context.set_parent(this);
  context.before_eaches = this->before_eaches;
  context.after_eaches = this->after_eaches;
  return context.run();
}

template <class T>
bool Description::context(T subject,
                          std::function<void(ClassDescription<T>&)> body) {
  ClassContext<T> context(body);
  context.subject = subject;
  context.set_parent(this);
  context.before_eaches = this->before_eaches;
  context.after_eaches = this->after_eaches;
  return context.run();
}

// template <class T>
// ClassContext<T>& Description::context(
//     T& subject, std::function<void(ClassDescription<T>&)> body) {
//   return context(subject, body);
// }

template <class T, typename U>
bool Description::context(std::initializer_list<U> init_list,
                          std::function<void(ClassDescription<T>&)> body) {
  ClassContext<T> context(T(init_list), body);
  context.set_parent(this);
  context.before_eaches = this->before_eaches;
  context.after_eaches = this->after_eaches;
  return context.run();
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
bool ClassDescription<T>::it(std::string name,
                             std::function<void(ItCd<T>&)> body) {
  ItCd<T> it(this->subject, name, body);
  it.set_parent(this);
  bool result = it.run();
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
bool ClassDescription<T>::it(std::function<void(ItCd<T>&)> body) {
  ItCd<T> it(this->subject, body);
  it.set_parent(this);
  bool result = it.run();
  exec_after_eaches();
  exec_before_eaches();
  return result;
}

template <class T>
bool ClassDescription<T>::run() {
  std::cout << padding() << descr << std::endl;
  body(*this);
  std::cout << std::endl;
  return this->get_status();
}

template <class T>
Expectations::Expectation<T> ItCd<T>::is_expected() {
  auto cd = static_cast<ClassDescription<T>*>(this->get_parent());
  Expectations::Expectation<T> expectation(cd->subject);
  expectation.set_parent(this);
  return expectation;
}

template <class T>
bool ItCd<T>::run() {
  if (!this->needs_descr()) {
    std::cout << padding() << get_descr() << std::endl;
  }
  body(*this);
  auto cd = static_cast<ClassDescription<T>*>(this->get_parent());
  cd->reset_lets();
  return this->get_status();
}

#endif /* CLASS_DESCRIPTION_H */

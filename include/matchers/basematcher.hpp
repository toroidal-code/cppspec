#ifndef BASEMATCHER_H
#define BASEMATCHER_H

#include "pretty_matchers.hpp"
#include "expectations/handler.hpp"
#include "it_base.hpp"

namespace Expectations {
template <class T>
class Expectation;
}

using namespace Expectations;

namespace Matchers {
template <typename Actual, typename Expected>
class BaseMatcher : public Runnable, public Pretty {
  std::string message = "";

 protected:
  Expected expected;
  Expectation<Actual> expectation;

 public:
  BaseMatcher(BaseMatcher<Actual, Expected> const &copy)
      : Runnable(copy.get_parent()),
        message(copy.message),
        expected(copy.expected),
        expectation(copy.expectation){};

  BaseMatcher(Expectation<Actual> &expectation)
      : Runnable(*expectation.get_parent()),  // We want the parent of the
                                              // matcher to be the `it` block,
                                              // not the
                                              // Expectation.
        expectation(expectation) {}

  BaseMatcher(Expectation<Actual> &expectation, Expected expected)
      : Runnable(*expectation.get_parent()),
        expected(expected),
        expectation(expectation) {}

  virtual bool match() = 0;
  virtual bool negated_match() { return !match(); }
  virtual std::string failure_message();
  virtual std::string failure_message_when_negated();
  virtual std::string description();
  Actual &get_actual() { return expectation.get_target(); }
  Expected &get_expected() { return expected; }
  Expectation<Actual> &get_expectation() { return expectation; }
  virtual BaseMatcher &set_message(std::string message) {
    this->message = message;
    return *this;
  }
  Result run() override;

  typedef Expected expected_t;
};

template <typename A, typename E>
std::string BaseMatcher<A, E>::failure_message() {
  std::stringstream ss;
  ss << "expected " << get_actual() << " to " << description();
  return ss.str();
}

template <typename A, typename E>
std::string BaseMatcher<A, E>::failure_message_when_negated() {
  std::stringstream ss;
  ss << "expected " << get_actual() << " to not " << description();
  return ss.str();
}

template <typename A, typename E>
std::string BaseMatcher<A, E>::description() {
  std::stringstream ss;
  std::string pretty_expected = this->to_sentance(expected);
  ss << "match " << this->name_to_sentance(Util::demangle(typeid(*this).name()))
     << "(" << pretty_expected.substr(1, pretty_expected.length()) << ")";
  return ss.str();
}

template <typename A, typename E>
Result BaseMatcher<A, E>::run() {
  ItBase *par = static_cast<ItBase *>(this->get_parent());

  // If we need a description for our test, generate it
  // unless we're ignoring the output.
  if (par->needs_descr() && !expectation.get_ignore_failure()) {
    std::cout << par->padding() << "should "
              << (expectation.get_sign() ? "" : "not ") << this->description()
              << std::endl;
  }

  Result matched =
      expectation.get_sign()
          ? PositiveExpectationHandler::handle_matcher<A>(*this, this->message)
          : NegativeExpectationHandler::handle_matcher<A>(*this, this->message);

  // If our items didn't match, we obviously failed.
  // Only report the failure if we aren't actively ignoring it.
  if (!matched && !expectation.get_ignore_failure()) this->failed();

  return matched;
}
}

#endif /* BASEMATCHER_H */

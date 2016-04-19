#include "cppspec.hpp"

// Very simple int<=>int custom matcher
struct CustomMatcher : public Matchers::BaseMatcher<int, int> {
  CustomMatcher(Expectations::Expectation<int> &expectation, int expected)
      : Matchers::BaseMatcher<int,int>(expectation, expected){};
  bool match() { return get_expected() == get_actual(); }
};

describe expectation_spec("Expectation", $ {
  context("Extensible .to", _ {
    it("accepts a custom BaseMatcher subclass", _ {
      auto e = expect(2);
      e.to(CustomMatcher(e, 2));
    });
  });
});

int main() { return expectation_spec.run() ? EXIT_SUCCESS : EXIT_FAILURE; }
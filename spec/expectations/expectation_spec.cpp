#include "cppspec.hpp"

// Very simple int<=>int custom matcher
struct CustomMatcher : public Matchers::BaseMatcher<int, int> {
  CustomMatcher(Expectations::Expectation<int> &expectation, int expected)
      : Matchers::BaseMatcher<int,int>(expectation, expected){};
  bool match() { return get_expected() == get_actual(); }
};

bool lam() { return true; }

describe expectation_spec("Expectation", $ {
  context("Extensible .to", _ {
    it("accepts a custom BaseMatcher subclass", _ {
      auto e = expect(2);
      e.to(CustomMatcher(e, 2));
    });
  });

  context(".ignore()", [=] (Description &self) {
    ItD i(self, _ {});
#undef expect
    let(e, [&] { return i.expect(5); });
#define expect self.expect

    it("flips the ignore_failure flag", [&e](auto &self) {
      expect(e->get_ignore_failure()).to_equal(false);
      expect(e->ignore().get_ignore_failure()).to_equal(true);
    });

    it("makes it so that matches do not alter the status of the parent", [&e](auto &self)  {
      expect([&]{
        e->ignore().to_equal(4);
        return self.get_status();
      }).to_be_true();
    });
  });
});

int main() { return expectation_spec.run() ? EXIT_SUCCESS : EXIT_FAILURE; }
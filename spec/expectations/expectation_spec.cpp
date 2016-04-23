#include "cppspec.hpp"

// Very simple int<=>int custom matcher
struct CustomMatcher : public Matchers::BaseMatcher<int, int> {
  CustomMatcher(Expectations::Expectation<int> &expectation, int expected)
      : Matchers::BaseMatcher<int,int>(expectation, expected){};
  bool match() { return get_expected() == get_actual(); }
};

describe expectation_spec("Expectation", $ {
  context(".to", _ {
    it("accepts a custom BaseMatcher subclass", _ {
      auto e = expect(2);
      e.to(CustomMatcher(e, 2));
    });
  });

  context(".to_be_true", _ {
    it("is true when true", _ {
      expect(true).to_be_true();
    });

    it("is false when false", _ {
      expect(false).ignore().to_be_true();
    });
  });

  context(".to_be_false", _ {
    it("is true when false", _ {
      expect(false).to_be_false();
    });

    it("is false when true", _ {
      expect(true).ignore().to_be_false();
    });
  });

  context(".to_fail", _ {
    it("is true when Result is false", _ {
      expect(Result::failure).to_fail();
    });

    it("is false when Result is true", _ {
      expect(expect(Result::success).ignore().to_fail().get_status()).to_be_false();
      expect(expect(Result::success).ignore().to_fail()).to_fail();
    });
  });

  context(".to_fail_with", _ {
    it("is true when Result is false and messages match", _ {
      expect(Result::failure_with("failure")).to_fail_with("failure");
    });

    context("is false when Result", _ {
      it("is false and messages don't match", _ {
        expect(
          expect(Result::failure_with("fail")).ignore().to_fail_with("failure").get_status()
        ).to_be_false();
      });

      it("is true and messages match", _ {
        expect(
            expect(Result::success_with("failure")).ignore().to_fail_with("failure").get_status()
        ).to_be_false();
      });

      it("is true and messages don't match", _ {
        expect(
            expect(Result::success_with("fail")).ignore().to_fail_with("failure").get_status()
        ).to_be_false();
      });
    });
  });

  context(".ignore()", [=] (Description &self) {
    ItD i(self, _ {});
#undef expect
    // TODO: Allow lets take a &self that refers to calling it?
    let(e, [&] { return i.expect(5); });
#define expect self.expect

//    it("flips the ignore_failure flag", _ {
//      expect(e->get_ignore_failure()).to_equal(false);
//      expect(e->ignore().get_ignore_failure()).to_equal(true);
//    });

//    it("makes it so that matches do not alter the status of the parent", _ {
//      expect([&] {
//        e->ignore().to_equal(4);
//        return i.get_status();
//      }).to_be_true();

//      std::cout << std::boolalpha << expect([&self] {
//        auto e = ItD(*(self.get_parent()), _ {}).expect(5);
//        auto r = e->to_equal(4);
//        std::cout << std::boolalpha << r.value << std::endl;
//        std::cout << std::boolalpha << i.get_status() << std::endl;
//        return i.get_status();
//      }).get_target() << std::endl;
//    });
  });
});

int main() { return expectation_spec.run(Printer::terse) ? EXIT_SUCCESS : EXIT_FAILURE; }
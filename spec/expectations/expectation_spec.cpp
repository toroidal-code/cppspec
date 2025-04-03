#include "cppspec.hpp"

using namespace CppSpec;

// Very simple int<=>int custom matcher
struct CustomMatcher : public Matchers::MatcherBase<int, int> {
  CustomMatcher(Expectation<int> &expectation, int expected)
      : Matchers::MatcherBase<int,int>(expectation, expected){};
  bool match() { return expected() == actual(); }
};

// clang-format off
describe expectation_spec("Expectation", $ {
  context(".to", _ {
    it("accepts a custom MatcherBase subclass", _ {
      auto e = expect(2);
      e.to(CustomMatcher(e, 2));
    });
  });

  context(".to_be_true", _ {
    it("is true when true", _ {
      expect(true).to_be_true();
    });

    it("is false when false", _ {
      expect(false).not_().to_be_true();
    });
  });

  context(".to_be_false", _ {
    it("is true when false", _ {
      expect(false).to_be_false();
    });

    it("is false when true", _ {
      expect(true).not_().to_be_false();
    });
  });

  context(".to_fail", _ {
    it("is true when Result is false", _ {
      expect(Result::failure(std::source_location::current())).to_fail();
    });

    it("is false when Result is true", _ {
      //expect(expect(Result::success(std::source_location::current())).ignore().to_fail().get_status()).to_be_false();
      //expect(expect(Result::success(std::source_location::current())).ignore().to_fail()).to_fail();
    });
  });

  context(".to_fail_with", _ {
    it("is true when Result is false and messages match", _ {
      expect(Result::failure_with(std::source_location::current(), "failure")).to_fail_with("failure");
    });

    context("is false when Result", _ {
      it("is false and messages don't match", _ {
        //expect(
          //expect(Result::failure_with(std::source_location::current(), "fail")).ignore().to_fail_with("failure").get_status()
        //).to_be_false();
      });

      it("is true and messages match", _ {
        //expect(
            //expect(Result::success_with(std::source_location::current(), "failure")).ignore().to_fail_with("failure").get_status()
        //).to_be_false();
      });

      it("is true and messages don't match", _ {
        //expect(
            //expect(Result::success_with(std::source_location::current(), "fail")).ignore().to_fail_with("failure").get_status()
        //).to_be_false();
      });
    });
  });

  context(".ignore()", _ {
    ItD i(self, std::source_location::current(), _ {});
#undef expect
    // TODO: Allow lets take a &self that refers to calling it?
    let(e, [&] { return i.expect(5); });
#define expect self.expect

    it("flips the ignore_failure flag", _ {
      expect(e->ignore_failure()).to_be_false();
      expect(e->ignore().ignore_failure()).to_be_true();
    });

    it("makes it so that matches do not alter the status of the parent", _ {
      expect([=]() mutable {
        e->ignore().to_equal(4);
        return i.get_result().status();
      }).to_be_true();
    });

    it("still returns Result::failure on match failure", _ {
      // expect([=]() mutable {
      //   return e->ignore().to_equal(4);
      // }).to_fail();
    });
  });

  context("ExpectationFunc", _ {
	it("is lazy", _{
	  // MSVCC optimizes this away into an int, when
	  // we explicitly want a function. Any other time
	  // that would be perfectly okay.
      std::function<int()> foo = [] { return 1 + 2; };
      ExpectationFunc<decltype(foo)> expectation(self, foo, std::source_location::current());
      expect(expectation.get_target()).to_equal(3);
    });
  });
});

CPPSPEC_MAIN(expectation_spec);

#include <cstddef>
#include <cstdint>
#include <limits>
#include <source_location>

#include "cppspec.hpp"

using namespace CppSpec;

// Whether the matcher fails, run outside any example so the failure is not recorded.
template <template <typename, typename> class M, typename A, typename E>
bool fails(A actual, E bound, bool negated) {
  auto ex = ExpectationValue(actual, std::source_location::current());
  if (negated) {
    ex.not_();
  }
  return M<A, E>(ex, bound).run().is_failure();
}

// clang-format off
describe numeric_spec("numeric comparison matchers", $ {
  context("to_be_greater_than", _ {
    it("passes when actual > expected (int)", _ {
      expect(5).to_be_greater_than(3);
    });

    it("fails when actual == expected", _ {
      expect(3).not_().to_be_greater_than(3);
    });

    it("fails when actual < expected", _ {
      expect(1).not_().to_be_greater_than(5);
    });

    it("works with doubles", _ {
      expect(3.14).to_be_greater_than(3.0);
    });

    it("works with negative numbers", _ {
      expect(-1).to_be_greater_than(-5);
    });

    it("works with zero boundary", _ {
      expect(0).not_().to_be_greater_than(0);
      expect(1).to_be_greater_than(0);
      expect(-1).not_().to_be_greater_than(0);
    });
  });

  context("to_be_less_than", _ {
    it("passes when actual < expected (int)", _ {
      expect(2).to_be_less_than(5);
    });

    it("fails when actual == expected", _ {
      expect(5).not_().to_be_less_than(5);
    });

    it("fails when actual > expected", _ {
      expect(9).not_().to_be_less_than(4);
    });

    it("works with doubles", _ {
      expect(2.71).to_be_less_than(3.14);
    });

    it("works with negative numbers", _ {
      expect(-10).to_be_less_than(-1);
    });

    it("works with zero boundary", _ {
      expect(0).not_().to_be_less_than(0);
      expect(-1).to_be_less_than(0);
      expect(1).not_().to_be_less_than(0);
    });
  });

  context("with a signed and an unsigned integer", _ {
    it("compares them by value", _ {
      expect(-1).not_().to_be_greater_than(0u);
      expect(0u).to_be_greater_than(-1);
      expect(-1).to_be_less_than(0u);
      expect(std::size_t{3}).not_().to_be_less_than(-1);
      expect(std::numeric_limits<std::uint64_t>::max()).to_be_greater_than(std::int64_t{-1});
      expect(std::int8_t{-1}).to_be_less_than(std::uint8_t{1});
    });

    it("compares characters and bools as the operators do", _ {
      expect('b').to_be_greater_than('a');
      expect(false).to_be_less_than(true);
    });
  });

  context("with NaN", _ {
    const double nan = std::numeric_limits<double>::quiet_NaN();

    it("fails to_be_greater_than, negated or not", _ {
      expect(fails<Matchers::BeGreaterThan>(nan, 0.0, false)).to_be_true();
      expect(fails<Matchers::BeGreaterThan>(nan, 0.0, true)).to_be_true();
      expect(fails<Matchers::BeGreaterThan>(0.0, nan, true)).to_be_true();
    });

    it("fails to_be_less_than, negated or not", _ {
      expect(fails<Matchers::BeLessThan>(nan, 0.0, false)).to_be_true();
      expect(fails<Matchers::BeLessThan>(nan, 0.0, true)).to_be_true();
      expect(fails<Matchers::BeLessThan>(0.0, nan, true)).to_be_true();
    });
  });

  context("combined usage", _ {
    it("value is between two bounds via two expectations", _ {
      int v = 7;
      expect(v).to_be_greater_than(5);
      expect(v).to_be_less_than(10);
    });

    it("float comparison chain", _ {
      double pi = 3.14159;
      expect(pi).to_be_greater_than(3.0);
      expect(pi).to_be_less_than(4.0);
    });
  });
});

CPPSPEC_MAIN(numeric_spec);

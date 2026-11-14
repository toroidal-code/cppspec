#include <cstddef>
#include <limits>
#include <source_location>

#include "cppspec.hpp"
using namespace CppSpec;

// Whether to_be_between fails, run outside any example so the failure is not recorded.
template <typename A, typename E>
bool between_fails(A actual, E min, E max, Matchers::RangeMode mode, bool negated) {
  auto ex = ExpectationValue(actual, std::source_location::current());
  if (negated) {
    ex.not_();
  }
  return Matchers::BeBetween<A, E>(ex, min, max, mode).run().is_failure();
}

// clang-format off
describe be_between_spec("BeBetween", $ {
//  class SizeMatters : public Matchers::BaseMatcher<std::string, std::string> {
//    SizeMatters(Expectation<std::string> &expectation, std::string expected)
//        : Matchers::BaseMatcher<std::string,std::string>(expectation, expected){};
//    bool match() { return this->actual() == this->expected(); }
//  };

  context("expect(...).to_be_between(min, max) (inclusive)", _ {
//    it_behaves_like("be_between", &Matchers::BeBetween::inclusive, _ {
//      Matchers::BeBetween matcher(auto min, auto max) {
//        return Matchers::BeBetween(self, min, max);
//      }
//    });

    it("is inclusive", _ {
      expect(1).to_be_between(1, 10);
      expect(10).to_be_between(1, 10);
    });
  });

  context("expect(...).to_be_between(min, max) (exclusive)", _ {
//    it_behaves_like("be_between", &Matchers::BeBetween::inclusive, _ {
//      Matchers::BeBetween matcher(auto min, auto max) {
//        return Matchers::BeBetween(self, min, max);
//      }
//    });

    it("is not inclusive", _ {
      expect(1).not_().to_be_between(1, 10, Matchers::RangeMode::exclusive);
      expect(10).not_().to_be_between(1, 10, Matchers::RangeMode::exclusive);
    });
  });

  context("with a signed and an unsigned integer", _ {
    it("compares them by value (inclusive)", _ {
      expect(std::size_t{3}).to_be_between(-1, 5);
      expect(-2).not_().to_be_between(0u, std::numeric_limits<unsigned>::max());
    });

    it("compares them by value (exclusive)", _ {
      expect(std::size_t{0}).to_be_between(-1, 1, Matchers::RangeMode::exclusive);
      expect(-2).not_().to_be_between(0u, std::numeric_limits<unsigned>::max(), Matchers::RangeMode::exclusive);
    });
  });

  context("with NaN", _ {
    const double nan = std::numeric_limits<double>::quiet_NaN();

    it("fails, negated or not (inclusive)", _ {
      expect(between_fails(nan, 0.0, 1.0, Matchers::RangeMode::inclusive, false)).to_be_true();
      expect(between_fails(nan, 0.0, 1.0, Matchers::RangeMode::inclusive, true)).to_be_true();
    });

    it("fails, negated or not (exclusive)", _ {
      expect(between_fails(nan, 0.0, 1.0, Matchers::RangeMode::exclusive, false)).to_be_true();
      expect(between_fails(nan, 0.0, 1.0, Matchers::RangeMode::exclusive, true)).to_be_true();
    });
  });

});

CPPSPEC_MAIN(be_between_spec);

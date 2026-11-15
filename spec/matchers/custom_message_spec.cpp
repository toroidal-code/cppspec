#include <format>
#include <functional>
#include <source_location>
#include <stdexcept>
#include <string>
#include <vector>

#include "cppspec.hpp"

using namespace CppSpec;

namespace {

// What a matcher reports when it fails, with the custom message set first when one is given.
template <typename Matcher>
std::string report(Matcher matcher, const std::string& message = "") {
  if (!message.empty()) {
    matcher.set_message(message);
  }
  return matcher.run().get_message();
}

// An expectation on a value that is negated before any matcher sees it.
template <typename T>
ExpectationValue<T> negated(T value) {
  ExpectationValue<T> expectation(value, std::source_location::current());
  expectation.not_();
  return expectation;
}

// An expectation whose target throws when it is read, as a block that throws does.
class Throwing : public Expectation<int> {
 public:
  Throwing() : Expectation<int>(std::source_location::current()) {}
  int& get_target() & override { throw std::runtime_error("boom"); }
  Throwing& not_() override {
    is_positive_ = !is_positive_;
    return *this;
  }
  Throwing& ignore() override {
    ignore_ = true;
    return *this;
  }
};

// A matcher as a user writes one: its own failure messages, and no word about a custom one.
class IsEven : public Matchers::MatcherBase<int, int> {
 public:
  explicit IsEven(Expectation<int>& expectation) : MatcherBase(expectation, 0) {}
  std::string verb() override { return "be even"; }
  std::string failure_message() override { return std::format("expected {} to be even", actual()); }
  std::string failure_message_when_negated() override { return std::format("expected {} to be odd", actual()); }
  bool match() override { return actual() % 2 == 0; }
};

// A matcher that forgets to say anything when it fails.
class Silent : public Matchers::MatcherBase<int, int> {
 public:
  explicit Silent(Expectation<int>& expectation) : MatcherBase(expectation, 0) {}
  std::string failure_message() override { return ""; }
  bool match() override { return false; }
};

const std::string empty_message_diagnostic =
    "Failure message is empty. Does your matcher define the appropriate failure_message[_when_negated] method to "
    "return a string?";

}  // namespace

// clang-format off
describe custom_message_spec("A matcher's custom failure message", $ {
  context("on Equal", _ {
    it("leads the own message, which still prints both values", _ {
      auto ex = ExpectationValue(1, std::source_location::current());
      const std::string own = report(Matchers::Equal<int, int>(ex, 2));
      expect(own).to_start_with(std::string("expected (int) => 2"));
      expect(report(Matchers::Equal<int, int>(ex, 2), "which button")).to_equal("which button\n" + own);
    });

    it("leads the negated message", _ {
      auto ex = negated(1);
      const std::string own = report(Matchers::Equal<int, int>(ex, 1));
      expect(report(Matchers::Equal<int, int>(ex, 1), "which button")).to_equal("which button\n" + own);
    });
  });

  context("on Satisfy", _ {
    it("leads the own message", _ {
      const auto above_five = [](int x) { return x > 5; };
      auto ex = ExpectationValue(3, std::source_location::current());
      const std::string own = report(Matchers::Satisfy<int>(ex, above_five));
      expect(report(Matchers::Satisfy<int>(ex, above_five), "the count")).to_equal("the count\n" + own);
    });

    it("leads the negated message", _ {
      const auto above_five = [](int x) { return x > 5; };
      auto ex = negated(7);
      const std::string own = report(Matchers::Satisfy<int>(ex, above_five));
      expect(report(Matchers::Satisfy<int>(ex, above_five), "the count")).to_equal("the count\n" + own);
    });
  });

  context("on BeWithin", _ {
    it("leads the own message", _ {
      auto ex = ExpectationValue(4.0, std::source_location::current());
      const std::string own = report(Matchers::BeWithin<double, double>(ex, 0.5, 5.0, ""));
      expect(report(Matchers::BeWithin<double, double>(ex, 0.5, 5.0, ""), "the gain")).to_equal("the gain\n" + own);
    });

    it("leads the negated message", _ {
      auto ex = negated(4.0);
      const std::string own = report(Matchers::BeWithin<double, double>(ex, 0.5, 4.2, ""));
      expect(report(Matchers::BeWithin<double, double>(ex, 0.5, 4.2, ""), "the gain")).to_equal("the gain\n" + own);
    });

    it("leads the own message when given to to_be_within(tolerance, message)", _ {
      auto ex = ExpectationValue(4.0, std::source_location::current());
      const std::string own = report(Matchers::BeWithin<double, double>(ex, 0.5, 5.0, ""));
      Matchers::BeWithinHelper<double, double> helper(ex, 0.5);
      helper.set_message("the gain");
      expect(helper.of(5.0).full_failure_message()).to_equal("the gain\n" + own);
    });
  });

  context("on Throw", _ {
    it("leads the own message", _ {
      using NoThrow = Matchers::Throw<std::function<int()>, std::exception>;
      auto ex = ExpectationValue(std::function<int()>([] { return 42; }), std::source_location::current());
      const std::string own = report(NoThrow(ex));
      expect(report(NoThrow(ex), "the load")).to_equal("the load\n" + own);
    });

    it("leads the negated message", _ {
      Throwing ex;
      ex.not_();
      const std::string own = report(Matchers::Throw<int, std::exception>(ex));
      expect(report(Matchers::Throw<int, std::exception>(ex), "the load")).to_equal("the load\n" + own);
    });
  });

  context("on Contain, which builds on the base's message", _ {
    it("leads the own message once", _ {
      using ContainInt = Matchers::Contain<std::vector<int>, int, int>;
      auto ex = ExpectationValue(std::vector{1, 2, 3}, std::source_location::current());
      const std::string own = report(ContainInt(ex, 4));
      expect(report(ContainInt(ex, 4), "the pads")).to_equal("the pads\n" + own);
    });

    it("leads the negated message once", _ {
      using ContainInt = Matchers::Contain<std::vector<int>, int, int>;
      auto ex = negated(std::vector{1, 2, 3});
      const std::string own = report(ContainInt(ex, 2));
      expect(report(ContainInt(ex, 2), "the pads")).to_equal("the pads\n" + own);
    });
  });

  context("on a matcher that keeps the base's messages", _ {
    it("leads the own message, which still prints its values", _ {
      auto ex = ExpectationValue(3, std::source_location::current());
      const std::string own = report(Matchers::BeLessThan<int, int>(ex, 2));
      expect(report(Matchers::BeLessThan<int, int>(ex, 2), "the level")).to_equal("the level\n" + own);
    });

    it("leads the negated message, which still prints its values", _ {
      auto ex = negated(1);
      const std::string own = report(Matchers::BeLessThan<int, int>(ex, 2));
      expect(own).to_equal(std::string("expected 1 to not be less than 2"));
      expect(report(Matchers::BeLessThan<int, int>(ex, 2), "the level")).to_equal("the level\n" + own);
    });
  });

  context("on a matcher that defines its own failure messages", _ {
    it("leads the own message", _ {
      auto ex = ExpectationValue(3, std::source_location::current());
      expect(report(IsEven(ex), "the step")).to_equal(std::string("the step\nexpected 3 to be even"));
    });

    it("leads the negated message", _ {
      auto ex = negated(4);
      expect(report(IsEven(ex), "the step")).to_equal(std::string("the step\nexpected 4 to be odd"));
    });
  });

  context("on a matcher whose own message is empty", _ {
    it("is replaced by the empty-message diagnostic", _ {
      auto ex = ExpectationValue(3, std::source_location::current());
      expect(report(Silent(ex))).to_equal(empty_message_diagnostic);
    });

    it("leaves the empty-message diagnostic below it, with no blank line", _ {
      auto ex = ExpectationValue(3, std::source_location::current());
      expect(report(Silent(ex), "the step")).to_equal("the step\n" + empty_message_diagnostic);
    });
  });
});

CPPSPEC_MAIN(custom_message_spec);

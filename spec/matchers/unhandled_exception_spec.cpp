#include "cppspec.hpp"

using namespace CppSpec;

class UnhandledExceptionMatcher : public CppSpec::Matchers::MatcherBase<int, int> {
 public:
  UnhandledExceptionMatcher(CppSpec::Expectation<int>& expectation, int expected)
      : MatcherBase(expectation, expected) {}

  bool match() override { throw std::runtime_error("Unhandled exception"); }

  std::string failure_message() override { return "Expected no exception"; }
  std::string failure_message_when_negated() override { return "Expected exception"; }
  std::string description() override { return "unhandled exception"; }
};
;

// clang-format off
describe unhandled_exception_spec("Unhandled exceptions", $ {
  it("are treated as errors", _ {
    ExpectationValue<int> expectation(0, std::source_location::current());
    UnhandledExceptionMatcher matcher(expectation, 0);
    Result result = matcher.run();
    expect(result.is_error()).to_equal(true);
    expect(result.get_message()).to_equal("Unhandled exception");
  });
});


CPPSPEC_MAIN(unhandled_exception_spec);

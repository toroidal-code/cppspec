#include <source_location>
#include <string>

#include "cppspec.hpp"

using namespace CppSpec;

// clang-format off
describe description_spec("A matcher's default description", $ {
  it("puts one space between the verb and the expected value", _ {
    auto ex = ExpectationValue(3, std::source_location::current());
    expect(Matchers::BeLessThan<int, int>(ex, 2).description()).to_equal(std::string("be less than 2"));
    expect(Matchers::BeGreaterThan<int, int>(ex, 4).description()).to_equal(std::string("be greater than 4"));
  });

  it("reads as one sentence in the failure message", _ {
    auto ex = ExpectationValue(3, std::source_location::current());
    expect(Matchers::BeLessThan<int, int>(ex, 2).run().get_message())
        .to_equal(std::string("expected 3 to be less than 2"));
  });
});

CPPSPEC_MAIN(description_spec);

#include <format>
#include <source_location>
#include <sstream>
#include <string>

#include "cppspec.hpp"
#include "formatters/verbose.hpp"

using namespace CppSpec;

namespace {
std::string format_verbose(const ItBase& example) {
  std::ostringstream out;
  Formatters::Verbose formatter(out);
  formatter.set_color_output(false);
  formatter.format(example);
  return out.str();
}
}  // namespace

// clang-format off
describe verbose_spec("Formatters::Verbose", $ {
  it("prints a failed expectation's file and line beside its message", _ {
    auto location = std::source_location::current();
    ItD example(location, "fails", [](ItD&) {});
    example.add_result(Result::failure_with(location, "expected 1\n     got 2"));

    expect(format_verbose(example))
        .to_equal(std::format("fails\n{}:{}: expected 1\n     got 2\n", location.file_name(), location.line()));
  });

  it("prints only the description of a passing example", _ {
    auto location = std::source_location::current();
    ItD example(location, "passes", [](ItD&) {});
    example.add_result(Result::success(location));

    expect(format_verbose(example)).to_equal(std::string("passes\n"));
  });
});

CPPSPEC_MAIN(verbose_spec);

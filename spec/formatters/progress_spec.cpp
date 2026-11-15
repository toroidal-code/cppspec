#include <format>
#include <source_location>
#include <sstream>
#include <string>

#include "cppspec.hpp"
#include "formatters/progress.hpp"

using namespace CppSpec;

namespace {
// Formats one example of a one-level group with the progress formatter and returns everything it prints: the status
// character, then the failure list.
std::string format_progress(const ItBase& example) {
  std::ostringstream out;
  Formatters::Progress formatter(out);
  formatter.set_color_output(false);
  formatter.format(example);
  formatter.format_failure_messages();
  return out.str();
}
}  // namespace

// clang-format off
describe progress_spec("Formatters::Progress", $ {
  it("lists a failed expectation's file and line beside its message, and nothing for a passing one", _ {
    auto location = std::source_location::current();
    Description group(location, std::string("group"));
    auto* example = group.make_child<ItD>(location, "fails", [](ItD&) {});
    example->add_result(Result::success(location));
    example->add_result(Result::failure_with(location, "the label\nexpected 1\n     got 2"));
    example->add_result(Result::success(location));

    expect(format_progress(*example))
        .to_equal(std::format("F\nTest number 1 failed:group\n  fails\n{}:{}: the label\nexpected 1\n     got 2\n",
                              location.file_name(), location.line()));
  });

  it("lists nothing for a skipped expectation, with a message or without, beside a failure", _ {
    auto location = std::source_location::current();
    Description group(location, std::string("group"));
    auto* example = group.make_child<ItD>(location, "fails", [](ItD&) {});
    example->add_result(Result::skipped(location));
    example->add_result(Result::skipped_with(location, "an ignored failure"));
    example->add_result(Result::failure_with(location, "the failure"));

    expect(format_progress(*example))
        .to_equal(std::format("F\nTest number 1 failed:group\n  fails\n{}:{}: the failure\n", location.file_name(),
                              location.line()));
  });

  it("lists nothing for a passing example", _ {
    auto location = std::source_location::current();
    Description group(location, std::string("group"));
    auto* example = group.make_child<ItD>(location, "passes", [](ItD&) {});
    example->add_result(Result::success(location));

    expect(format_progress(*example)).to_equal(std::string(".\n"));
  });
});

CPPSPEC_MAIN(progress_spec);

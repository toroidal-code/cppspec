#include "cppspec.hpp"
#include "formatters/verbose.hpp"

extern std::function<void(CppSpec::Runner &)> expectation_spec_group,
    be_between_spec_group, be_within_spec_group, equal_spec_group,
    describe_a_spec_group, describe_spec_group;

int main(void) {
  CppSpec::Runner runner{};
  expectation_spec_group(runner);
  be_between_spec_group(runner);
  be_within_spec_group(runner);
  equal_spec_group(runner);
  describe_a_spec_group(runner);
  describe_spec_group(runner);

  return runner.exec<CppSpec::Formatters::Verbose>() ? EXIT_SUCCESS : EXIT_FAILURE;
}

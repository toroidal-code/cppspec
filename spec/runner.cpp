#include "cppspec.hpp"

extern CppSpec::Runner::spec_group expectation_spec_group,
    be_between_spec_group, be_within_spec_group, equal_spec_group,
    describe_a_spec_group, describe_spec_group;

int main(void) {
  return CppSpec::Runner(CppSpec::Formatters::progress)
                 .add_spec_group(expectation_spec_group)
                 .add_spec_group(be_between_spec_group)
                 .add_spec_group(be_within_spec_group)
                 .add_spec_group(equal_spec_group)
                 .add_spec_group(describe_a_spec_group)
                 .add_spec_group(describe_spec_group)
                 .exec()
             ? EXIT_SUCCESS
             : EXIT_FAILURE;
}

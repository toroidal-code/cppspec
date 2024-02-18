#include <cstdlib>

#include "cppspec.hpp"


using namespace CppSpec;

struct TestClass {
  std::string field1;
  std::string field2;
  TestClass() : field1("foo"), field2("bar"){};
  TestClass(std::string field1, std::string field2)
      : field1(field1), field2(field2){};
};

describe_a<TestClass> describe_a_implicit_spec(
    "Implicit subjects", $ {
      it(
          "Default constructor should be called", _ {
            expect(subject.field1).to_equal("foo");
            expect(subject.field2).to_equal("bar");
          });
    });

describe_a<TestClass> describe_a_explicit_spec(
    "Explicit subjects", TestClass("bar", "baz"), $ {
      it(
          "Specified constructor should be called", _ {
            expect(subject.field1).to_equal("bar");
            expect(subject.field2).to_equal("baz");
          });
    });

describe_a<std::true_type> describe_a_syntax_spec(
    "describe_a syntax", $ {
      context(
          "A nested context with no given subject", _ {
            it(
                "should inherit the subject",
                _ { expect(subject).to_equal(true); });
          });

      context<int>(
          "A nested context with a given subject", 1, _ {
            it(
                "should not have the same subject as the parent",
                _ { is_expected().to_equal(1); });
          });
    });

int main(int argc, char **argv) {
  return CppSpec ::parse(argc, argv)
                 .add_spec(describe_a_implicit_spec)
                 .add_spec(describe_a_explicit_spec)
                 .add_spec(describe_a_syntax_spec)
                 .exec()
             ? EXIT_SUCCESS
             : EXIT_FAILURE;
}

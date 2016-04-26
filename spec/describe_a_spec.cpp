#include "cppspec.hpp"

using namespace CppSpec;

struct TestClass {
  std::string field1;
  std::string field2;
  TestClass() : field1("foo"), field2("bar"){};
  TestClass(std::string field1, std::string field2)
      : field1(field1), field2(field2){};
};

describe_a <TestClass> describe_a_implicit_spec("Implicit subjects", $ {
  it("Default constructor should be called", _ {
    expect(subject.field1).to_equal("foo");
    expect(subject.field2).to_equal("bar");
  });
});

describe_a <TestClass> describe_a_explicit_spec(
    "Explicit subjects", TestClass("bar", "baz"), $ {
  it("Specified constructor should be called", _ {
    expect(subject.field1).to_equal("bar");
    expect(subject.field2).to_equal("baz");
  });
});


Runner::spec_group describe_a_spec_group([](Runner &r){
  r.add_spec(describe_a_implicit_spec)
   .add_spec(describe_a_explicit_spec);
});
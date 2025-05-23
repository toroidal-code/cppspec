#include "cppspec.hpp"

using namespace CppSpec;

// clang-format off
describe equal_spec("Equal", $ {
  it("matches when actual == expected", _ {
    expect(1).to_equal(1);
  });

  it("does not match when actual != expected", _ {
    expect(1).not_().to_equal(2);
  });

  //it("compares by sending == to actual (not expected)", _ {
    //static bool called = false;
    //struct Actual {
    //  bool operator==(const Actual &other) {
    //    called = true;
    //    return true;
    //  }
    // };

    //Actual actual;
    //expect(actual);
  //});
});


CPPSPEC_MAIN(equal_spec);

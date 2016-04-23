#include "cppspec.hpp"

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

int main() { return equal_spec.run(Printer::terse) ? EXIT_SUCCESS : EXIT_FAILURE; }

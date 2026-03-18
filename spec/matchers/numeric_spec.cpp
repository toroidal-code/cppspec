#include "cppspec.hpp"

using namespace CppSpec;

describe numeric_spec("numeric comparison matchers", $ {
  context("to_be_greater_than", _ {
    it("passes when actual > expected (int)", _ {
      expect(5).to_be_greater_than(3);
    });

    it("fails when actual == expected", _ {
      expect(3).not_().to_be_greater_than(3);
    });

    it("fails when actual < expected", _ {
      expect(1).not_().to_be_greater_than(5);
    });

    it("works with doubles", _ {
      expect(3.14).to_be_greater_than(3.0);
    });

    it("works with negative numbers", _ {
      expect(-1).to_be_greater_than(-5);
    });

    it("works with zero boundary", _ {
      expect(0).not_().to_be_greater_than(0);
      expect(1).to_be_greater_than(0);
      expect(-1).not_().to_be_greater_than(0);
    });
  });

  context("to_be_less_than", _ {
    it("passes when actual < expected (int)", _ {
      expect(2).to_be_less_than(5);
    });

    it("fails when actual == expected", _ {
      expect(5).not_().to_be_less_than(5);
    });

    it("fails when actual > expected", _ {
      expect(9).not_().to_be_less_than(4);
    });

    it("works with doubles", _ {
      expect(2.71).to_be_less_than(3.14);
    });

    it("works with negative numbers", _ {
      expect(-10).to_be_less_than(-1);
    });

    it("works with zero boundary", _ {
      expect(0).not_().to_be_less_than(0);
      expect(-1).to_be_less_than(0);
      expect(1).not_().to_be_less_than(0);
    });
  });

  context("combined usage", _ {
    it("value is between two bounds via two expectations", _ {
      int v = 7;
      expect(v).to_be_greater_than(5);
      expect(v).to_be_less_than(10);
    });

    it("float comparison chain", _ {
      double pi = 3.14159;
      expect(pi).to_be_greater_than(3.0);
      expect(pi).to_be_less_than(4.0);
    });
  });
});

CPPSPEC_MAIN(numeric_spec);

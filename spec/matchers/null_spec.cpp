#include "cppspec.hpp"

using namespace CppSpec;

describe null_spec("to_be_null matcher", $ {
  context("with raw pointers", _ {
    it("passes when pointer is null", _ {
      int* p = nullptr;
      expect(p).to_be_null();
    });

    it("fails when pointer is non-null", _ {
      int x = 42;
      int* p = &x;
      expect(p).not_().to_be_null();
    });

    it("passes not_().to_be_null() for valid pointer", _ {
      double d = 3.14;
      expect(&d).not_().to_be_null();
    });
  });

  context("with void pointer", _ {
    it("passes for null void pointer", _ {
      void* p = nullptr;
      expect(p).to_be_null();
    });

    it("fails for non-null void pointer", _ {
      int x = 0;
      void* p = &x;
      expect(p).not_().to_be_null();
    });
  });

  context("to_be_truthy and to_be_falsy (numeric types)", _ {
    it("zero int is falsy", _ {
      expect(0).to_be_falsy();
    });

    it("non-zero int is truthy", _ {
      expect(1).to_be_truthy();
      expect(-1).to_be_truthy();
    });

    it("zero double is falsy", _ {
      expect(0.0).to_be_falsy();
    });

    it("non-zero double is truthy", _ {
      expect(3.14).to_be_truthy();
    });
  });
});

CPPSPEC_MAIN(null_spec);

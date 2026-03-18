#include <cstdlib>

#include "cppspec.hpp"

using namespace CppSpec;

namespace {
int dsl_n         = 0;
int dsl_callcount = 0;
int dsl_x         = 0;
}  // namespace

// before_each deferred semantics
describe before_each_spec("before_each deferred semantics", $ {
  before_each([] { dsl_n = 42; });

  it("runs before_each before first it", _ {
    expect(dsl_n).to_equal(42);
    dsl_n = 99;
  });

  it("resets via before_each before second it", _ {
    expect(dsl_n).to_equal(42);
  });
});

// let memoization and reset
describe let_spec("let memoization", $ {
  let(val, [] { return ++dsl_callcount; });

  it("memoizes within an it block", _ {
    int a = *val;
    int b = *val;
    expect(a).to_equal(b);
  });

  it("resets between it blocks", _ {
    int v = *val;
    expect(v).to_equal(2);  // call_count incremented once per it
  });
});

// context nesting
describe context_spec("context nesting", $ {
  before_each([] { dsl_x = 1; });

  it("outer it sees before_each", _ {
    expect(dsl_x).to_equal(1);
  });

  context("nested context", _ {
    it("inner it also sees outer before_each", _ {
      expect(dsl_x).to_equal(1);
    });
  });
});

// describe_a typed subject
struct MyValue {
  int val;
  explicit MyValue(int v) : val(v) {}
};

describe_a<MyValue> describe_a_spec("describe_a", MyValue(42), $ {
  it("subject is accessible via subject keyword", _ {
    expect(subject.val).to_equal(42);
  });

  it("is_expected() works", _ {
    expect(subject.val).to_equal(42);
  });

  context("nested context inherits subject", _ {
    it("can still access subject", _ {
      expect(subject.val).to_equal(42);
    });
  });
});

CPPSPEC_MAIN(before_each_spec, let_spec, context_spec, describe_a_spec);

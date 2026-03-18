#include <cstdlib>
#include <string>
#include <vector>

#include "cppspec.hpp"

using namespace CppSpec;

namespace {
int let_calls_1  = 0;
int let_a_calls  = 0;
int let_b_calls  = 0;
}  // namespace

// Memoization within a single it block
describe let_memoize_spec("let memoization within an it", $ {
  let(expensive, [] { return ++let_calls_1; });

  it("calls factory only once per it", _ {
    int a = *expensive;
    int b = *expensive;
    int c = *expensive;
    expect(a).to_equal(b);
    expect(b).to_equal(c);
    expect(let_calls_1).to_equal(1);
  });

  it("resets between its: factory called again", _ {
    int v = *expensive;
    expect(let_calls_1).to_equal(2);
    expect(v).to_equal(2);
  });
});

// Multiple lets in the same describe
describe multiple_lets_spec("multiple lets in one describe", $ {
  let(a, [] { return ++let_a_calls; });
  let(b, [] { return ++let_b_calls * 10; });

  it("both lets are independent", _ {
    expect(*a).to_equal(1);
    expect(*b).to_equal(10);
    expect(*a).to_equal(1);   // memoized
    expect(*b).to_equal(10);  // memoized
  });

  it("both reset for next it", _ {
    expect(*a).to_equal(2);
    expect(*b).to_equal(20);
  });
});

// Let with a complex type
describe let_complex_type_spec("let with complex type", $ {
  let(vec, ([] { return std::vector<int>{1, 2, 3, 4, 5}; }));

  it("produces expected vector", _ {
    expect(*vec).to_contain(3);
    expect(*vec).to_start_with({1, 2});
    expect(*vec).to_end_with({4, 5});
  });

  it("each it gets a fresh vector", _ {
    vec->push_back(99);
    expect(*vec).to_contain(99);
  });

  it("modifications do not persist across its", _ {
    expect(*vec).not_().to_contain(99);
  });
});

// Let with string
describe let_string_spec("let with string", $ {
  let(greeting, [] { return std::string{"hello"}; });

  it("starts with expected prefix", _ {
    expect(*greeting).to_start_with("hel");
  });

  it("ends with expected suffix", _ {
    expect(*greeting).to_end_with("llo");
  });

  it("equality check", _ {
    expect(*greeting).to_equal("hello");
  });
});

// Let in nested context
describe let_nested_spec("let in nested context", $ {
  let(outer_val, [] { return 100; });

  it("outer it uses outer let", _ {
    expect(*outer_val).to_equal(100);
  });

  context("nested context with its own let", _ {
    let(inner_val, [] { return 200; });

    it("inner it uses inner let", _ {
      expect(*inner_val).to_equal(200);
    });

    it("inner it can also use outer let", _ {
      expect(*outer_val).to_equal(100);
      expect(*inner_val).to_equal(200);
    });
  });

  it("outer it after nested context: outer let still works", _ {
    expect(*outer_val).to_equal(100);
  });
});

CPPSPEC_MAIN(let_memoize_spec, multiple_lets_spec, let_complex_type_spec,
             let_string_spec, let_nested_spec);

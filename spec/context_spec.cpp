#include <cstdlib>
#include <string>

#include "cppspec.hpp"

using namespace CppSpec;

namespace {
int depth_val  = 0;
int prop_n     = 0;
int sib_shared = 0;
int explain_v  = 0;
}  // namespace

// Deep nesting: 3+ levels
describe deep_nesting_spec("deep nesting", $ {
  context("level 1", _ {
    depth_val = 1;

    context("level 2", _ {
      depth_val = 2;

      context("level 3", _ {
        depth_val = 3;

        it("is at depth 3", _ {
          expect(depth_val).to_equal(3);
        });
      });

      it("is at depth 3 after level-3 context ran", _ {
        expect(depth_val).to_equal(3);
      });
    });
  });
});

// before_each propagates through multiple levels
describe propagation_spec("before_each propagates through levels", $ {
  before_each([] { prop_n = 0; });  // level 0: reset to 0

  it("level-0 it: n == 0", _ {
    expect(prop_n).to_equal(0);
  });

  context("level 1", _ {
    before_each([] { prop_n += 1; });  // adds 1 after outer resets

    it("level-1 it: n == 1 (0 + 1)", _ {
      expect(prop_n).to_equal(1);
    });

    context("level 2", _ {
      before_each([] { prop_n += 10; });  // adds 10 after outer hooks

      it("level-2 it: n == 11 (0 + 1 + 10)", _ {
        expect(prop_n).to_equal(11);
      });

      it("each level-2 it is fresh: n == 11 again", _ {
        expect(prop_n).to_equal(11);
      });
    });

    it("back in level-1: n == 1 (level-2 hook not active here)", _ {
      expect(prop_n).to_equal(1);
    });
  });

  it("back in level-0: n == 0", _ {
    expect(prop_n).to_equal(0);
  });
});

// Multiple contexts at the same level are independent
describe sibling_contexts_spec("sibling contexts are independent", $ {
  before_each([] { sib_shared = 100; });

  context("context A", _ {
    before_each([] { sib_shared += 1; });

    it("A sees 101", _ {
      expect(sib_shared).to_equal(101);
    });
  });

  context("context B", _ {
    before_each([] { sib_shared += 2; });

    it("B sees 102", _ {
      expect(sib_shared).to_equal(102);
    });
  });

  context("context C (no extra hook)", _ {
    it("C sees 100 (only outer hook)", _ {
      expect(sib_shared).to_equal(100);
    });
  });
});

// Context with typed subject (ClassDescription created inline)
describe typed_context_spec("typed context via context(name, subject, body)", $ {
  context("int subject == 42", 42, _ {
    it("is_expected() returns 42", _ {
      is_expected().to_equal(42);
    });

    it("subject is greater than 0", _ {
      is_expected().to_be_greater_than(0);
    });
  });

  context("string subject", std::string{"cppspec"}, _ {
    it("subject starts with cpp", _ {
      is_expected().to_start_with("cpp");
    });

    it("subject ends with spec", _ {
      is_expected().to_end_with("spec");
    });
  });
});

// explain is an alias for context
describe explain_alias_spec("explain is an alias for context", $ {
  before_each([] { explain_v = 5; });

  explain("the value", _ {
    it("equals 5", _ {
      expect(explain_v).to_equal(5);
    });

    it("is greater than 0", _ {
      expect(explain_v).to_be_greater_than(0);
    });
  });
});

CPPSPEC_MAIN(deep_nesting_spec, propagation_spec, sibling_contexts_spec,
             typed_context_spec, explain_alias_spec);

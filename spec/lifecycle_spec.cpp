#include <cstdlib>

#include "cppspec.hpp"

using namespace CppSpec;

namespace {
int before_each_calls = 0;
int after_each_state  = 0;
int before_all_init   = 0;
int after_all_x       = 0;
int hook_n            = 0;
int stacked_total     = 0;
}  // namespace

// before_each runs once per it, not once per describe
describe before_each_ordering_spec("before_each ordering", $ {
  before_each([] { before_each_calls++; });

  it("first it: calls == 1", _ {
    expect(before_each_calls).to_equal(1);
  });

  it("second it: calls == 2 (incremented again)", _ {
    expect(before_each_calls).to_equal(2);
  });

  it("third it: calls == 3", _ {
    expect(before_each_calls).to_equal(3);
  });
});

// after_each runs after every it
describe after_each_ordering_spec("after_each ordering", $ {
  before_each([] { after_each_state = 10; });
  after_each([] { after_each_state = 0;  });

  it("sees state == 10 at start", _ {
    expect(after_each_state).to_equal(10);
    after_each_state = 99;  // mutate — after_each resets it
  });

  it("sees state == 10 again (reset by after_each)", _ {
    expect(after_each_state).to_equal(10);
  });

  it("state is always clean", _ {
    expect(after_each_state).to_equal(10);
  });
});

// before_all runs exactly once at the start of the block
describe before_all_spec("before_all runs once", $ {
  before_all([] { before_all_init = 42; });

  it("sees the before_all value", _ {
    expect(before_all_init).to_equal(42);
  });

  it("value is not reset between its", _ {
    expect(before_all_init).to_equal(42);
    before_all_init = 99;
  });

  it("now sees the modified value (before_all did NOT re-run)", _ {
    expect(before_all_init).to_equal(99);
  });
});

// after_all fires after all its in a context are done
describe after_all_timing_spec("after_all timing", $ {
  context("inner context with after_all", _ {
    after_all([] { after_all_x = 777; });

    it("runs while x is still 0", _ {
      expect(after_all_x).to_equal(0);
    });

    it("still 0 before after_all fires", _ {
      expect(after_all_x).to_equal(0);
    });
    // after_all fires here
  });

  it("outer it sees x == 777 after inner after_all ran", _ {
    expect(after_all_x).to_equal(777);
  });
});

// hooks propagate into nested contexts
describe hook_propagation_spec("hook propagation into nested contexts", $ {
  before_each([] { hook_n = 1; });
  after_each([] { hook_n = 0; });

  it("outer it sees n == 1", _ {
    expect(hook_n).to_equal(1);
  });

  context("nested context inherits outer hooks", _ {
    it("inner it also sees n == 1 from outer before_each", _ {
      expect(hook_n).to_equal(1);
    });

    it("inner it gets fresh n each time", _ {
      expect(hook_n).to_equal(1);
      hook_n = 99;
    });

    it("after_each reset: n is 1 again", _ {
      expect(hook_n).to_equal(1);
    });
  });

  it("outer it after nested still gets n == 1", _ {
    expect(hook_n).to_equal(1);
  });
});

// additional before_each in nested context stacks on top of parent
describe stacked_hooks_spec("stacked hooks in nested contexts", $ {
  before_each([] { stacked_total = 0; stacked_total += 1; });  // outer: reset then add 1

  context("inner adds more", _ {
    before_each([] { stacked_total += 10; });  // runs after outer: 0 + 1 + 10 = 11

    it("sees total == 11 (1 from outer + 10 from inner)", _ {
      expect(stacked_total).to_equal(11);
    });

    it("each it starts fresh: total == 11 again", _ {
      expect(stacked_total).to_equal(11);
    });
  });

  it("outer it only gets outer before_each: total == 1", _ {
    expect(stacked_total).to_equal(1);
  });
});

CPPSPEC_MAIN(before_each_ordering_spec, after_each_ordering_spec, before_all_spec,
             after_all_timing_spec, hook_propagation_spec, stacked_hooks_spec);

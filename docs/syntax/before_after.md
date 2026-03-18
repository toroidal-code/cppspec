# before_each, after_each, before_all, after_all

C++Spec provides four lifecycle hooks to manage setup and teardown around examples.

## before_each

Runs once **before every `it`** in the enclosing `describe` or `context`:

```cpp
namespace { int n = 0; }

describe lifecycle_spec("lifecycle", $ {
  before_each([] { n = 0; });

  it("starts at zero", _ {
    expect(n).to_equal(0);
  });

  it("can be set to 5", _ {
    n = 5;
    expect(n).to_equal(5);
  });

  it("is zero again (reset by before_each)", _ {
    expect(n).to_equal(0);
  });
});
```

## after_each

Runs once **after every `it`** in the enclosing block. Useful for releasing resources or
asserting post-conditions:

```cpp
namespace { std::FILE* f = nullptr; }

describe file_spec("File handle", $ {
  before_each([] { f = std::tmpfile(); });
  after_each([]  { if (f) { std::fclose(f); f = nullptr; } });

  it("is open after setup", _ {
    expect(f).not_().to_be_null();
  });
});
```

## before_all

Runs **once** before the first `it` in the enclosing block. Unlike `before_each`, it does
**not** re-run between examples — mutations made inside examples persist:

```cpp
namespace { int init_count = 0; }

describe before_all_spec("Expensive setup", $ {
  before_all([] { init_count = 42; });

  it("sees the value set by before_all", _ {
    expect(init_count).to_equal(42);
  });

  it("mutations persist (before_all does not re-run)", _ {
    init_count = 99;
    expect(init_count).to_equal(99);
  });

  it("sees the mutated value from the previous it", _ {
    expect(init_count).to_equal(99);
  });
});
```

Use `before_all` only when setup is genuinely expensive and examples do not mutate shared
state, or when mutation is intentional (as above).

## after_all

Runs **once** after the last `it` in the enclosing block:

```cpp
namespace { int x = 0; }

describe after_all_spec("Timing", $ {
  context("inner context", _ {
    after_all([] { x = 777; });

    it("x is still 0 while its run", _ { expect(x).to_equal(0); });
    it("x is still 0 here too",      _ { expect(x).to_equal(0); });
    // after_all fires here → x = 777
  });

  it("outer it sees x == 777 after inner context completed", _ {
    expect(x).to_equal(777);
  });
});
```

## Hook inheritance in nested contexts

Hooks defined in a parent `describe` or `context` run for all `it` blocks in child contexts
as well. Additional hooks registered in a child context run **after** the parent's, stacking
up:

```cpp
namespace { int total = 0; }

describe stacked_spec("Stacked hooks", $ {
  before_each([] { total = 0; total += 1; });  // runs first: total = 1

  context("inner", _ {
    before_each([] { total += 10; });           // runs second: total = 11

    it("sees 11", _ {
      expect(total).to_equal(11);
    });
  });

  it("outer it sees only 1 (inner before_each does not apply)", _ {
    expect(total).to_equal(1);
  });
});
```

Hook execution order for a nested `it`:

1. Parent `before_each` hooks (outermost first)
2. Child `before_each` hooks
3. The `it` body runs
4. Child `after_each` hooks
5. Parent `after_each` hooks (outermost last)

# let

`let` introduces a *memoized* value inside a `describe` or `context` block. The factory
lambda is called at most once per `it` — subsequent accesses within the same example return
the cached value. The cache is cleared automatically before the next `it` runs.

## Basic usage

```cpp
namespace { int _count = 0; }

describe let_spec("let", $ {
  let(count, [] { return ++_count; });

  it("memoizes the value within an it", _ {
    expect(count).to_equal(1);
    expect(count).to_equal(1);  // same call — factory not invoked again
  });

  it("resets between its", _ {
    expect(count).to_equal(2);  // factory called fresh for this it
  });
});
```

`let` creates a `Let<T>&`. Dereference it with `*` to obtain the value, or use `->` to call
members directly. For simple types, `expect(count)` resolves via the `Let<T>&` overload of
`expect` and calls `value()` automatically.

```cpp
let(greeting, [] { return std::string{"hello"}; });

it("uses arrow access", _ {
  expect(greeting->size()).to_equal(5u);
});
```

## Why use let instead of a plain variable?

A plain variable shared via capture is shared across all `it` blocks — mutation in one
example leaks into the next. A `let` value is always freshly computed at the start of each
`it`, so examples remain independent.

```cpp
// Fragile: shared variable must be reset manually
namespace { std::vector<int> v; }
before_each([] { v = {1, 2, 3}; });

// Clean: automatically fresh each it
let(v, ([] { return std::vector<int>{1, 2, 3}; }));
```

## let in nested contexts

Each `context` can define its own `let` values alongside those inherited from its parent:

```cpp
describe nested_let_spec("nested let", $ {
  let(base, [] { return 10; });

  it("outer sees base", _ {
    expect(*base).to_equal(10);
  });

  context("inner", _ {
    let(derived, [] { return 20; });

    it("inner sees both", _ {
      expect(*base).to_equal(10);
      expect(*derived).to_equal(20);
    });
  });
});
```

!!! note

    When the `let` factory lambda contains commas (e.g. an initializer list), wrap the
    lambda in parentheses to prevent the preprocessor from treating the commas as macro
    argument separators:

    ```cpp
    let(vec, ([] { return std::vector<int>{1, 2, 3}; }));
    ```

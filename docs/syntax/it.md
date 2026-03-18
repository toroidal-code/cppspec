# it

`it` blocks are the individual examples of a spec. Each `it` should contain at least one
expectation that verifies a specific behaviour.

## Named it

The most common form takes a description string and a block:

```cpp
it("returns the correct sum", _ {
  expect(1 + 1).to_equal(2);
});
```

The description appears in test output and should read like plain English when prefixed by its
enclosing `describe` or `context` name.

!!! important

    Use `_` for all `it` bodies. `_` expands to `[=](auto& self) mutable -> void`.

## Anonymous it

Omit the description to have C++Spec generate one automatically from the contained expectation:

```cpp
it(_ {
  expect(answer).to_equal(42);
});
// Output: "is expected to equal 42"
```

## it inside describe_a

Inside a `describe_a<T>` block, the `subject` keyword and `is_expected()` are available:

```cpp
describe_a<std::string> str_spec(std::string{"hello"}, $ {
  it("has the right length", _ {
    expect(subject.size()).to_equal(5u);
  });

  it(_ {
    is_expected().to_start_with("hel");
  });
});
```

## specify

`specify` is an alias for `it`:

```cpp
specify("correct total price", _ {
  expect(cart.total()).to_equal(9.99);
});
```

## Nesting with context

`it` blocks live inside `describe`, `describe_a`, or `context` blocks. `context` (and its alias
`explain`) groups related examples and can inherit lifecycle hooks from its parent:

```cpp
describe stack_spec("Stack", $ {
  context("when empty", _ {
    it("has size 0", _ { expect(stack.size()).to_equal(0); });
  });

  context("after one push", _ {
    before_each([] { stack.push(1); });
    it("has size 1", _ { expect(stack.size()).to_equal(1); });
  });
});
```

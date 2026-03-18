Every test suite begins with either `describe` or `describe_a`.

# describe

`describe` creates a named test suite:

```cpp
describe example_spec("An example", $ {
  // it blocks and hooks go here
});
```

Each `describe` is a global instance of `Description`. The `$` macro expands to
`[](auto& self) -> void`. Everything inside the block (`it`, `context`, `before_each`, etc.)
resolves through `self` automatically via macros, so you write them unqualified.

!!! important

    Use `$` for `describe` and `context` bodies. Use `_` for `it` bodies.

In expanded form, the above is equivalent to:

```cpp
Description example_spec("An example", [](auto& self) -> void { });
```

State shared between hooks and examples can live as a local variable inside the `$` block or
as a file-scope variable in an anonymous namespace:

```cpp
namespace { int n = 0; }

describe counter_spec("Counter", $ {
  before_each([] { n = 0; });

  it("starts at zero", _ {
    expect(n).to_equal(0);
  });
});
```

# describe_a

`describe_a<T>` creates a typed test suite with a *subject* — an instance of `T` available
to all examples. The template parameter determines the subject type.

```cpp
template <typename T>
class ClassDescription : public Description { ... };
```

## Implicit subject

When no value is provided, `T` is default-constructed:

```cpp
describe_a<MyClass> my_spec("MyClass", $ {
  it("starts in a valid state", _ {
    expect(subject.is_valid()).to_be_true();
  });
});
```

## Explicit subject

Pass a value after the description string:

```cpp
describe_a<Point> point_spec("Point{3,4}", Point{3.0, 4.0}, $ {
  it("has length 5", _ {
    expect(subject.length()).to_be_within(1e-9).of(5.0);
  });
});
```

For containers, an initializer list is also accepted:

```cpp
describe_a<std::vector<int>> vec_spec({1, 2, 3}, $ {
  it("contains 2", _ {
    expect(subject).to_contain(2);
  });
});
```

## Accessing the subject

Inside a `describe_a<T>` block there are two ways to access the subject:

**`subject` keyword** — available inside any `it` body at any nesting depth:

```cpp
it("accesses the subject", _ {
  expect(subject.value()).to_equal(42);
});
```

**`is_expected()`** — shorthand for `expect(subject)`, reads naturally when asserting on
the subject itself:

```cpp
it("has the right value", _ {
  is_expected().to_equal(MyClass{42});
});
```

## describe_an

`describe_an` is an alias for `describe_a` for grammatical convenience:

```cpp
describe_an<Animal> animal_spec("Animal", $ { ... });
```

# Entry point

Each spec file needs an entry point:

```cpp
CPPSPEC_MAIN(my_spec);                       // single suite
CPPSPEC_MAIN(spec_a, spec_b, spec_c);        // multiple suites
```

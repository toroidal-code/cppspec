
# Let

A `let` introduces a memoized local variable inside of a `Description`. This is best shown in an
example:

```c++
int _count = 0;
describe let_spec("let", $ {
  let(count, [&]{ return ++_count ;});

  it("memoizes the value", _ {
    expect(count).to_equal(1);
    expect(count).to_equal(1);
  });

  it("is not cached across examples", _ {
    expect(count).to_equal(2);
  });
});
```

As shown, `let`s allow setting a mutating variable before each `it`.

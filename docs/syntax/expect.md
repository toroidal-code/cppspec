# Expect

Expectations are the primary focus of C++Spec, allowing clean, readable tests without falling
back on assertions.

Expectations have two parts, the "actual" value, and the "expected" value.

```c++
auto x = 2;
expect(x).to_equal(2);
//     ^           ^
//	actual      expected
```

The `expect` half references whatever is being tested (the "actual" result of the test), while the
matcher references what the value _should_ be (the "expected" value of the test).

## Values

The standard usage of an `expect` is to reference a variable or value to test.

```c++
Something some_thing();

expect(some_thing.some_method()).to_be_true();
```

## Lambdas

Expectations are also to contain lambdas that return objects or throw exceptions.

```c++
expect([] { return 2; }).to_equal(2);
expect([] { throw std::exception; }).to_throw<std::exception>();
```

This allows creating delayed "thunks" that can be created earlier and then passed to the 
`Expectation`.

```c++
auto val = [] { return 5; };
expect(val).to_equal(5);
```

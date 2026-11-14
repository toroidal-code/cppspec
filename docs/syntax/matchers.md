# Matchers

Matchers are the verbs of a C++Spec expectation: `expect(actual).to_equal(expected)`.
Every matcher can be negated by inserting `.not_()` before it:

```cpp
expect(x).to_equal(0);          // passes when x == 0
expect(x).not_().to_equal(0);   // passes when x != 0
```

Every matcher also takes an optional last argument, a message printed above the matcher's own
text when the expectation fails (see [Expect](expect.md#custom-messages)):

```cpp
expect(gain).to_equal(2, "the gain");
```

Text in any form (`std::string`, `std::string_view`, a char array or a C string) is read
through `CppSpec::string_view_of()`; see [Expect](expect.md#c-strings) for how C strings, null
ones included, are compared and printed.

---

## Equality & boolean

### to_equal

Compares using `operator==`. Works with any type that defines equality:

```cpp
expect(1 + 1).to_equal(2);
expect(std::string{"hi"}).to_equal("hi");
```

Two pieces of text are compared as text, whatever form each is in, so two C strings with the same
text at different addresses are equal. A null C string equals only another null C string.

### to_be_true / to_be_false

Strict boolean check (value must be exactly `true` or `false`):

```cpp
expect(vec.empty()).to_be_true();
expect(result.ok()).to_be_false();
```

### to_be_truthy / to_be_falsy

Checks the result of `static_cast<bool>(value)`:

```cpp
expect(1).to_be_truthy();
expect(0).to_be_falsy();
expect(std::optional<int>{42}).to_be_truthy();
```

### to_be_null

Passes when a pointer is `nullptr`:

```cpp
int* p = nullptr;
expect(p).to_be_null();

int x = 1;
expect(&x).not_().to_be_null();

const char* name = nullptr;     // a C string stays a pointer, so this works too
expect(name).to_be_null();
```

---

## Containers

### to_contain (single element)

Passes when the container holds the given element (uses `std::ranges::find`):

```cpp
expect(std::vector<int>{1, 2, 3}).to_contain(2);
expect(std::string{"hello"}).to_contain('e');
expect("hello").to_contain("ell");
```

On text, a null C string, as the text searched or the text looked for, fails both positive and
negated. In a container that is not text, such as `std::vector<const char*>`, a null pointer is an
element like any other.

### to_contain (multiple elements)

Passes when **all** listed elements are present. Negated form passes when **none** are present:

```cpp
expect(std::vector<int>{1, 2, 3, 4, 5}).to_contain({1, 3, 5});   // all must be in vec
expect(std::vector<int>{1, 2, 3}).not_().to_contain({7, 8, 9});   // none may be in vec
```

### to_start_with

For strings, checks the leading prefix. For containers, checks the leading sub-sequence:

```cpp
expect(std::string{"hello world"}).to_start_with("hello");
expect(std::string_view{"hello world"}).to_start_with(std::string{"hello"});
expect(std::vector<int>{1, 2, 3, 4}).to_start_with({1, 2});
expect(std::vector<char>{'a', 'b', 'c'}).to_start_with("ab");
```

The text may be in any form `string_view_of()` reads. Of the ranges that are not text, only a range
of `char`, such as `std::vector<char>`, takes text to compare with; for any other range,
`to_start_with("ab")` does not compile. A null C string, on either side, fails both positive and
negated.

### to_end_with

For strings, checks the trailing suffix. For containers, checks the trailing sub-sequence:

```cpp
expect(std::string{"hello world"}).to_end_with("world");
expect(std::vector<int>{1, 2, 3, 4}).to_end_with({3, 4});
```

As with `to_start_with`, a null C string on either side fails both positive and negated.

---

## Numeric

### to_be_between

Passes when the value falls within the given range (inclusive by default):

```cpp
expect(5).to_be_between(1, 10);
expect(5).to_be_between(1, 10, RangeMode::exclusive);  // strict bounds
```

Negated, it passes only for a value below the lower bound or above the upper one.

### to_be_within

Floating-point proximity check. Use `.of(expected)` to specify the target:

```cpp
expect(3.14159).to_be_within(0.001).of(std::numbers::pi);
```

### to_be_less_than / to_be_greater_than

```cpp
expect(3).to_be_less_than(5);
expect(7).to_be_greater_than(4);
```

`to_be_less_than`, `to_be_greater_than` and `to_be_between` compare two integers by value, as `std::cmp_less` does,
so a negative number is below every unsigned one: `expect(-1).to_be_less_than(0u)` passes. `bool` and the character
types, and every non-integer type, are compared with the operators instead. Negated, each checks the strict opposite
(`not_().to_be_greater_than(x)` is `actual <= x`), so NaN fails these matchers negated or not, and a type compared
negated needs that opposite operator too.

---

## Strings

### to_match

Full regex match against `std::regex`. Accepts a pattern string or a pre-built `std::regex`:

```cpp
expect(std::string{"hello123"}).to_match("[a-z]+[0-9]+");
expect(std::string{"HELLO"}).to_match(std::regex("hello", std::regex::icase));
```

Note: `to_match` uses `std::regex_match` which requires the **entire** string to match the
pattern.

### to_partially_match

Passes when the pattern matches **somewhere** in the string (`std::regex_search`):

```cpp
expect(std::string{"hello123"}).to_partially_match("[0-9]+");
```

For both, the pattern may be given as text in any form `string_view_of()` reads. A null C string,
as the text or as the pattern, fails both positive and negated.

---

## Custom predicate

### to_satisfy

Accepts any callable `(const T&) -> bool`. Useful when no built-in matcher fits:

```cpp
expect(42).to_satisfy([](int n) { return n % 2 == 0; });
```

---

## Exceptions

`to_throw` requires the expected value to be a **non-void callable** (use
`std::function<void*()>` if needed).

### to_throw

Passes when the callable throws any exception:

```cpp
std::function<void*()> f = [] -> void* { throw std::runtime_error("boom"); };
expect(f).to_throw();
```

### to_throw\<E\>

Passes when the callable throws an exception of type `E` (or derived from `E`). Inside a
generic `_` block, use `.template to_throw<E>()`:

```cpp
it("catches the specific type", _ {
  std::function<void*()> f = [] -> void* { throw MyException{}; };
  expect(f).template to_throw<MyException>();
  expect(f).template to_throw<std::exception>();  // base class also matches
});
```

---

## std::optional / std::expected

### to_have_value

Passes when an `optional` or `expected` holds a value:

```cpp
std::optional<int> opt{42};
expect(opt).to_have_value();

std::expected<int, std::string> result = 42;
expect(result).to_have_value();
```

### to_have_error

Passes when a `std::expected` holds an error:

```cpp
std::expected<int, std::string> err = std::unexpected{"oops"};
expect(err).to_have_error();
```

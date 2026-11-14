# Expect

Expectations are the primary focus of C++Spec, allowing clean, readable tests without falling
back on assertions.

Expectations have two parts, the "actual" value, and the "expected" value.

```cpp
auto x = 2;
expect(x).to_equal(2);
//     ^           ^
//	actual      expected
```

The `expect` half references whatever is being tested (the "actual" result of the test), while the
matcher references what the value _should_ be (the "expected" value of the test).

## Values

The standard usage of an `expect` is to reference a variable or value to test.

```cpp
Something some_thing();

expect(some_thing.some_method()).to_be_true();
```

## Lambdas

Expectations are also to contain lambdas that return objects or throw exceptions.

```cpp
expect([] { return 2; }).to_equal(2);
expect([] { throw std::exception; }).to_throw<std::exception>();
```

This allows creating delayed "thunks" that can be created earlier and then passed to the 
`Expectation`.

```cpp
auto val = [] { return 5; };
expect(val).to_equal(5);
```

## C strings

`expect` keeps a C string (a `char*`, a `const char*`, or a char array, which decays to one) as
the pointer it is, so pointer matchers such as `to_be_null` work on it:

```cpp
const char* name = nullptr;
expect(name).to_be_null();
```

The string matchers (`to_equal`, `to_contain`, `to_start_with`, `to_end_with`, `to_match` and
`to_partially_match`) read every piece of text through `CppSpec::string_view_of()`, whatever
form it is in: a `std::string`, a `std::string_view`, a char array or a char pointer. Two C
strings are therefore compared by their text, not by their addresses, and a C string equals a
`std::string` holding the same text.

A null C string is not text:

- it equals only another null C string, and differs from every piece of text, the empty string
  included, so `expect(name).not_().to_equal("")` passes;
- the matchers that search text (`to_contain`, `to_start_with`, `to_end_with`, `to_match`,
  `to_partially_match`) fail on it **both** positive and negated, whether it is the actual
  value or the text or pattern looked for, and say so below their message
  (`nullptr is not text, so there is nothing to search` or `... nothing to look for`);
- it prints as `nullptr`.

A char array given to `expect` decays to a pointer, so it is read as a C string, up to its first
NUL: it must be NUL-terminated. Wrap a fixed-size buffer that may not be in a
`std::string_view(buffer, size)`.

A `volatile char*`, a `signed char*`, an `unsigned char*` and a pointer to a wide character
type are not C strings: they are compared as pointers and printed as addresses (`nullptr` when
null).

## Custom messages

Every matcher takes an optional last argument, a message that describes the expectation. When
the expectation fails, the message is printed **above** the matcher's own text, which still
shows both values:

```cpp
expect(gain).to_equal(2, "the gain");
```

```
the gain
expected (int) => 2
     got (int) => 1
Compared using `==`
```

## How values are printed

A failure message prints each value by its `operator<<` where it has one, with these
exceptions, which also apply to the elements of a standard container, built-in array, pair or
tuple, and of any other range with no `operator<<` of its own (a range type with its own
`operator<<` is printed through it):

| Value | Printed as |
| ----- | ---------- |
| `bool` | `true` / `false` |
| `std::int8_t`, `std::uint8_t` (`signed char`, `unsigned char`) | the number: `65`, `-3` |
| `wchar_t`, `char8_t`, `char16_t`, `char32_t` | the number of the code unit: `97` |
| `char` | the character: `A` |
| an enum with no `operator<<` (scoped, `std::byte` included) | its type and number: `Colour(2)`, `std::byte(65)` |
| `std::optional<T>` | its value, or `std::nullopt` |
| `std::optional<std::optional<T>>` holding an optional | `std::optional(...)`, so `std::optional(std::nullopt)` reads apart from `std::nullopt` |
| a C string | its text, or `nullptr` |
| a pointer to `signed char`, `unsigned char` or a wide character | its address, or `nullptr` |

So a failed comparison of two byte buffers reads `[65, 67]` against `[65, 66]`, and a container
keeps its delimiters: `[1, 2]`, `{1, 2}` for a set, `(1, 2)` for a pair or tuple.

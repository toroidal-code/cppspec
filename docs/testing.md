# Testing

Running tests requires one or more spec objects. Each spec is independent and self-contained.

Specs are handed to `CppSpec::parse` which returns a runner. The runner executes all specs and
returns a `Result`. Use `CPPSPEC_MAIN` for the common single-file case.

## Formatters

There are a number of formatter options for printing to a terminal: `verbose`, `progress`, and
`tap`. `progress` prints a series of dots while `verbose` prints a fully RSpec-like list of
tests, colouring them to show their status and result.

Pass a formatter to `CppSpec::parse`:

```cpp
CppSpec::parse(argc, argv, CppSpec::Formatters::verbose)
```

## Example

```cpp
#include <cstdlib>
#include <cstring>
#include "cppspec.hpp"

describe strcmp_spec("int strcmp ( const char * str1, const char * str2 )", $ {
  auto greater_than_zero = [](int i){ return i >= 0; };
  auto less_than_zero    = [](int i){ return i < 0; };

  it("returns 0 only when strings are equal", _ {
    expect(strcmp("hello", "hello")).to_equal(0);
  });

  it("returns a negative integer when str1 is less than str2", _ {
    expect(strcmp("hello", "world")).to_satisfy(less_than_zero);
    expect(strcmp("0123", "1321431")).to_satisfy(less_than_zero);
  });

  it("returns a positive integer if str1 is greater than str2", _ {
    expect(strcmp("yellow", "world")).to_satisfy(greater_than_zero);
    expect(strcmp("9", "789")).to_satisfy(greater_than_zero);
  });
});

CPPSPEC_MAIN(strcmp_spec);
```

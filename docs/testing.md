# Testing

Running tests requires a `Description` object to provide the spec. Each spec is independent.

Tests are added to a `Runner` manually, which is passed a `Formatter` object on instantiation.

Once a spec has been added to a `Runner`, the runner is then executed, returning a `Result` object.

`Result` objects are able to be implicitly casted to `bool`, and can thus be used for returning
directly from the `main` function.


## Formatters

There are a number of `Formatter` subclasses for printing to a terminal, including `Verbose`, `Progress`, and `TAP`. `Progress` prints out as a serious of periods, while `Verbose` prints a 
fully RSpec-like list of tests, coloring them to show their status and result.

## Example

Here's an example spec and the associated runner:

```c++
#include <cstdlib>
#include "cppspec.hpp"

describe strcmp_spec("int strcmp ( const char * str1, const char * str2 )", $ {
  auto greater_than_zero = [](int i){return i>=0;};
  auto less_than_zero = [](int i){return i<0;};

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


int main(){
  return CppSpec::Runner(CppSpec::Formatters::verbose)
             .add_spec(strcmp_spec)
             .exec() ? EXIT_SUCCESS : EXIT_FAILURE;
}

```

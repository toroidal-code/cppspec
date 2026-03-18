# C++Spec
[![license](https://img.shields.io/badge/license-MIT-blue)](https://choosealicense.com/licenses/mit/)
[![GitHub Action](https://github.com/toroidal-code/cppspec/actions/workflows/test.yml/badge.svg)]()&nbsp;
[![GitHub release](https://img.shields.io/github/release/toroidal-code/cppspec.svg)](https://github.com/toroidal-code/cppspec/releases/latest)&nbsp;
[![Documentation Status](https://readthedocs.org/projects/cppspec/badge/?version=latest)](http://cppspec.readthedocs.org/en/latest/?badge=latest)

A behavior-driven development testing library for C++ with an RSpec-inspired DSL.

## Documentation

See [http://cppspec.readthedocs.org/](http://cppspec.readthedocs.org/) for full documentation and a tutorial.

## Requirements

C++Spec requires a compiler and standard library with C++23 support. Currently tested:

- LLVM/Clang 18 (on Linux, macOS, and Windows)
- GCC 14.2 (on Linux and macOS)
- MSVC 19.43 (on Windows)
- AppleClang 16 (on macOS)

__Note:__ Only spec files require C++23 (`-std=c++23`). No other part of an existing project's build needs modification.

## Usage

The recommended approach is to integrate C++Spec as a CMake subproject:

```cmake
FetchContent_Declare(
  cppspec
  GIT_REPOSITORY https://github.com/toroidal-code/cppspec
  GIT_TAG        VERSION
)
FetchContent_MakeAvailable(cppspec)

# Or using CPM
CPMAddPackage("gh:toroidal-code/cppspec@VERSION")
```

Spec files are picked up automatically with:

```cmake
discover_specs(specs_folder)
```

This creates a separate CTest executable for every file ending in `_spec.cpp` in the given directory (recursive).

## Introduction

If you've ever used RSpec or Jasmine, chances are you'll be familiar with C++Spec's syntax. For example, this is a C++Spec version of the first snippet on RSpec's [README](https://github.com/rspec/rspec-core/blob/master/README.md#basic-structure).

```cpp
#include "cppspec.hpp"
#include "order.hpp"

describe order_spec("Order", $ {
  it("sums the prices of its line items", _ {
    Order order;

    order.add_entry(LineItem().set_item(Item()
      .set_price(Money(1.11, Money::USD))
    ));

    order.add_entry(LineItem().set_item(Item()
      .set_price(Money(1.11, Money::USD))
      .set_quantity(2)
    ));

    expect(order.total()).to_equal(Money(5.55, Money::USD));
  });
});

CPPSPEC_MAIN(order_spec);
```

## FAQ

## Attribution
Heavily inspired by [RSpec](https://github.com/rspec) and [Jasmine](http://jasmine.github.io).

## Authors
Copyright © 2014-2024 Katherine Whitlock

## License
The project is licensed under the MIT License.

# C++Spec 
[![version](https://img.shields.io/badge/version-v0.0.0-blue.svg)]()
[![GitHub Action](https://github.com/toroidal-code/cppspec/actions/workflows/test.yml/badge.svg)]()&nbsp;
[![GitHub release](https://img.shields.io/github/release/toroidal-code/cppspec.svg)](https://github.com/toroidal-code/cppspec/releases/latest)&nbsp;
[![Github Releases](https://img.shields.io/github/downloads/toroidal-code/cppspec/latest/total.svg)]()&nbsp;
[![Documentation Status](https://readthedocs.org/projects/cppspec/badge/?version=latest)](http://cppspec.readthedocs.org/en/latest/?badge=latest)

A behavior-driven development testing library for C++ with an RSpec-inspired DSL.

## Warning! This is pre-release software and may be incomplete, contain bugs, and/or introduce major breaking changes within a short period of time

## Installation ##

C++Spec will be released as a single collated header-file that can be placed in any include path in your project. After that, all features are available via `#include "cppspec.hpp"`.

If you want to use the git repo for development or to integrate it into your own
project as a submodule, releases will also be available as tags. This project's
include folder should then be added to your project's include path. Again, all
functionality is exposed through `#include "cppspec.hpp"`.

If you want to manually generate the collated `cppspec.hpp` yourself, you can download the ccollate tool [here](https://raw.githubusercontent.com/toroidal-code/ccollate/master/ccollate.rb) and then run `./ccollate.rb include/cppspec.hpp > cppspec.hpp` in the 
toplevel directory of the C++Spec repo. A fully-featured `cppspec.hpp` file 
will then be available in the root of the project for usage.


## Documentation ##

See [http://cppspec.readthedocs.org/](http://cppspec.readthedocs.org/) for full documentation and a tutorial.

## Requirements ##

C++Spec requires a compiler with support for C++11 and polymorphic lambda expressions from C++14. This includes GCC >= 4.9, MSVCC >= 14.0, or clang >= 3.4. For other compilers check [this chart](http://en.cppreference.com/w/cpp/compiler_support).

__Note:__ Only the tests require being compiled with C++14 support (`-std=c++14`). No other part of an existing project's build must be modified.

## Introduction ##

If you've ever used RSpec or Jasmine, chances are you'll be familiar with C++Spec's syntax. For example, this is a C++Spec version of the first snippet on RSpec's [README](https://github.com/rspec/rspec-core/blob/master/README.md#basic-structure).

```cpp
#include "cppspec.hpp"
#include "order.hpp"

describe order_spec("Order", $ {
  it("sums the prices of its line items", _ {
    Order order();

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


int main(){
  return CppSpec::Runner(CppSpec::Formatters::verbose)
             .add_spec(order_spec)
             .exec() ? EXIT_SUCCESS : EXIT_FAILURE;
}

```

## FAQ ##

## Attribution ##
Heavily inspired by [RSpec](https://github.com/rspec) and [Jasmine](http://jasmine.github.io).

## Authors ##
Copyright © 2014-2016 Katherine Whitlock

## License ##
The project is licensed under the MIT License.

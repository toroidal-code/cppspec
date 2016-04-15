#C++Spec [![version](https://img.shields.io/badge/version-v0.0.0-blue.svg)]()
[![Build Status](https://travis-ci.org/toroidal-code/cppspec.svg?branch=master)](https://travis-ci.org/toroidal-code/cppspec)&nbsp; 
[![GitHub release](https://img.shields.io/github/release/toroidal-code/cppspec.svg)](https://github.com/toroidal-code/cppspec/releases/latest)&nbsp;
[![Github Releases](https://img.shields.io/github/downloads/toroidal-code/cppspec/latest/total.svg)]()&nbsp; 
[![Documentation Status](https://readthedocs.org/projects/cppspec/badge/?version=latest)](http://cppspec.readthedocs.org/en/latest/?badge=latest)

C++Spec is a behavior-driven development library for C++ with an RSpec-inspired DSL.

## Installation ##

C++Spec is available as a single collated header-file that can be placed in any include path in your project. After that, all features are available via `#include "cppspec.hpp"`.

## Documentation ##

See [http://cppspec.readthedocs.org/]() for all documentation.

## Testing ##

## Requirements ##

C++Spec requires a compiler with support for C++11 and polymorphic lambda expressions from C++14. This includes GCC >= 4.9, MSVCC >= 14.0, or clang >= 3.4. For other compilers check [this chart](http://en.cppreference.com/w/cpp/compiler_support).

__Note:__ Only the tests require being compiled with C++14 support (`-std=c++14`). No other part of an existing project's build must be modified.

## FAQ ##

## Attribution ##
Heavily inspired by [RSpec](https://github.com/rspec) and [Jasmine](http://jasmine.github.io).

## Authors ##
Copyright © 2014-2016 Katherine Whitlock

## License ##
The project is licensed under the MIT License.

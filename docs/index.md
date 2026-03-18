# C++Spec

C++Spec is a behavior-driven development library with an RSpec-inspired DSL.

-------------------------------------------------------------------------------

## Overview

C++Spec is a behavior-driven development library for C++ with an RSpec-inspired DSL. Designed
with ease of use and rapid prototyping in mind, C++Spec offers an alternative to traditional
testing libraries and frameworks.

Also see the [official GitHub pages site](http://toroidal-code.github.io/cppspec/) for more
information.

## Requirements

C++Spec requires a compiler and standard library with C++23 support. Currently tested:

- LLVM/Clang 18 (Linux, macOS, Windows)
- GCC 14.2 (Linux, macOS)
- MSVC 19.43 (Windows)
- AppleClang 16 (macOS)

## Installing

The recommended approach is to integrate C++Spec as a CMake subproject:

```cmake
include(FetchContent)
FetchContent_Declare(
  cppspec
  GIT_REPOSITORY https://github.com/toroidal-code/cppspec
  GIT_TAG        VERSION
)
FetchContent_MakeAvailable(cppspec)
```

Spec files are picked up automatically with:

```cmake
discover_specs(specs_folder)
```

This creates a separate CTest executable for every file ending in `_spec.cpp` in the given
directory (recursive).

Alternatively, clone the repository and add the `include/` directory to your include path:

```sh
git clone https://github.com/toroidal-code/cppspec.git
```

Then `#include "cppspec.hpp"` in your spec files.

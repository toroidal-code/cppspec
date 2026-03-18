---
layout: default
profile: true
---

C++Spec is a behavior-driven development library for C++ with an RSpec-inspired DSL. Designed with ease of use and rapid prototyping in mind, C++Spec offers an alternative to traditional testing libraries and frameworks. Some things that make C++Spec different than other testing libraries:

- A clean, readable syntax inspired by RSpec and Jasmine
- Use as a library, not a framework
- Easily extensible with custom matchers
- Support for the constructs you'd expect: `describe`, `context`, `it`, `expect`, and `let`
- Can automatically generate description strings based on your tests
- Cross-platform with no need to change complex build settings



## An example:

```cpp
describe_a<std::list<int>> int_list_spec("A list of ints", {1, 2, 3}, $ {
  it("is doubly-linked", _ {
    expect(subject.front()).to_equal(1);
    expect(subject.back()).to_equal(3);
  });

  it(_ { is_expected().to_contain(6); });
  it(_ { is_expected().to_contain({1, 2, 3}); });
  it(_ { is_expected().not_().to_contain(4); });
});
```

<p style="padding-bottom:0;margin-bottom:-.5em;">Using the Verbose formatter this outputs:</p>
<pre class="highlight" style="padding-top:0;padding-bottom:0;margin-bottom:-.5em">
<code>
<span>A list of ints</span>
<span class="sr">  is doubly-linked</span>
<span class="s1">  is expected to contain 6</span>
<span class="s1">expected [1,2,3] to contain 6</span>
<span class="sr">  is expected to contain 1, 2, and 3</span>
<span class="sr">  is expected not to contain 4</span>
</code>
</pre>

## Usage:

The recommended approach is to integrate C++Spec as a subproject via CMake's `FetchContent`:

```cmake
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
directory (recursive). Full documentation is available via the links at the top of this page.

## How does it work?

C++Spec uses templated classes and lambdas to build an execution tree at runtime. The `$`
and `_` macros expand to lambda literals that receive a typed `self` reference, giving `it`,
`expect`, `let`, and other DSL calls access to the enclosing description context. A formatter
visits each node when the tests run and prints the status and any failures.

## I'm getting really long compiler errors. What's going on?

Due to the library's use of templates and type-deduced lambdas, error messages from the
compiler can be difficult to understand. Errors also tend to cascade, making structures above
the problem code fail to compile and further obscuring what is actually wrong.

Usually the only information you need is the actual error itself, not the template
substitution notes. You can reduce the template backtrace with the flag
`-ftemplate-backtrace-limit=1` when compiling with GCC or Clang.



{% include footer.html %}

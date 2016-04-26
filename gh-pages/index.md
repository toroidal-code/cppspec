---
layout: default
profile: true
---

C++Spec is a behavior-driven development library for C++ with an RSpec-inspired DSL. Designed with ease of use and rapid prototyping in mind, C++Spec offers an alternative to traditional testing libraries and frameworks. Some of the core concepts:

- A clean, readable syntax
- As few macros as possibles
- Use as a library, not a framework
- Easily extensible with custom matchers.
- Support for the RSpec and Jasmine constructs you'd expect, such as describe, context, it, expect, and let.
- Can automatically generate documentation strings based on your tests

## An example:

```c++
describe_a <std::list<int>>
int_list_spec("A list of ints", {1,2,3}, $ {
  it("is doubly-linked", _ {
    expect(subject.front()).to_equal(1);
    expect(subject.back()).to_equal(3);
  });

  it(_{ is_expected().to_include(6); });
  it(_{ is_expected().to_include({1,2,3}); });
  it(_{ is_expected().not_().to_include(4); });
});
```

<p style="padding-bottom:0;margin-bottom:-.5em;">Using the Verbose formatter this outputs:</p>
<pre class="highlight" style="padding-top:0;padding-bottom:0;margin-bottom:-.5em">
<code>
<span>A list of ints</span>
<span class="sr">  is doubly-linked</span>
<span class="s1">  should include 6</span>
<span class="s1">expected [1,2,3] to include 6</span>
<span class="sr">  should include 1, 2, and 3</span>
<span class="sr">  should not include 4</span>
</code>
</pre>

## Usage:

Download the [header file]() and put it in your project either alongside your tests or in a folder that is in your `INCLUDE` path. Then, simply `#include "cppspec.hpp"` and you're ready to go.

## How does it work?

C++Spec utilizes templated classes and functions as well as C++14 features in order to automatically deduce the types of your objects and construct your tests.

Lambdas are passed to functions (such as `context` and `it`) in order to build an execution tree at runtime. A formatter object visits each node when the tests are run and prints the status of the tests and any errors.

## I'm getting really long errors. What's going on?

Due to how the library is constructed with both templates and auto-type lambdas, error messages from the compiler can be difficult to understand. Errors also tend to cascade and make the enclosing objects also fail, further obfuscating what's actually going wrong.

Usually, the only information you want is the actual error, not all of the template substitution notes. You can reduce the template backtrace by using the flag `-ftemplate-backtrace-limit=1` when compiling with GCC and Clang.



{% include footer.html %}

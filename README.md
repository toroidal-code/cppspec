#C++Spec
Have a taste of some RSpec in your C++.

```cpp
#include <cmath>
#include <cstdlib>
#include <list>
#include "cppspec.hpp"


describe fabs_spec("fabs", $ {
  // you can use the `explain` keyword to
  // group behavior and nest descriptions
  explain("argument is zero", _ {
    it("return zero", _ {
      expect(fabs(0)).to_equal(0);
    });
  });

  int n = 0;
  before_each([&]{ n = rand(); });

  // you can also use `context` instead of
  // `explain`, just like in RSpec
  context("argument is positive", _ {
    it("return positive", _ {
      expect(fabs(n)).to_equal(n, "fabs(" + std::to_string(n) + ") didn't equal " + std::to_string(n));
    });
  });

  explain("argument is negative", _ {
    it("return positive", _ {
      expect(fabs(-n)).to_equal(n);
    });
  });
});

// describe_a allows you to create specs with implied subjects,
// useful for testing specific classes and a mock instance
describe_a <std::list<int>> int_list_spec({1,2,3}, $ {
  // the `is_expected` function is used for matching an implied subject
  it(_{ is_expected().to_include(3); });
});

int main(void) {
  bool r = true;
  r &= fabs_spec.run();
  r &= int_list_spec.run();
  return r ? EXIT_SUCCESS : EXIT_FAILURE;
}

```

## Why?
Short answer: Because I got fed up with using assertions.

Long answer:

Because good BDD-testing on C and C++ is hard.

I come from a software-engineering background, and it's been driven through my head that
testing is one of (if not _the_) the most important thing about writing stable software.
So when I started doing C++ development seriously about three years ago, I was really
surprised to see that there was no flexible RSpec-like testing framework for a language
that is used daily by thousands (millions?).

I wanted a library that let me make comprehensive and readable BDD tests really, really easily,
but nothing fit what I was looking for.

So I decided to try and make it myself.

## How?
Unholy black magic.

## No, really. How?
Lots of templates. Templates everywhere. _Literally_ everywhere. It's why this library is header-only.

A single 'describe' is actually a local variable that has a runtime-created execution tree.
Calling `.run()` on the root recursively calls `.run()` on its' children.

A common expectation like `expect({1,2,3}).to_include({1,2});` is more like
```cpp
Expecatations::Expectation<std::vector<int>>({1,2,3}).to_include<std::vector<int>,std::vector<int>,int>({1,2})
```

Not fun. Cool. But definitely not fun.

## Why should I use this instead of some other testing framework?

So that your tests can look nice. Or because you want to use RSpec syntax for testing C or C++. Or because it's pretty simple to use, yet powerful thanks to the extensible matchers. There's a bunch of possible reasons.

...Or maybe because you find it cool and you'd like to hack on it and contribute? (pretty please?)

## It's Ugly!
Yeah, I know. It's not the best. Let's see you make it any prettier (and if you do, send me a pull request :P ).

In all honesty though, this was as close to RSpec or Jasmine syntax as I could get. I think I did
a pretty darn good job, if I do say so myself. Even if the library itself seriously needs
to be cleaned up.

## It's Beautiful!
Why, thank you! I certainly tried.

## What the heck are `$` and `_`!?
They're just syntax sugar for `[](auto &self)` and `[=](auto &self)` respectively, defined to avoid repetition and improve readability. `it([=](auto &self){ is_expected.to_equal(5); });` isn't exactly what I'm aiming for with syntax.

We need that `self` parameter to pass a reference to the containing structure. For example, `describe` 
blocks get a `Description` object, while `it` blocks get an `It` object. 

Annoyingly, you can't have an automatic capture-list on a non-local lambda 
(which is what a top-level `describe`'s body is). This means that there needs 
to be a separate one-character shortcut. If they really bother you, or you just 
want to be verbose, you can stick `#undef $` and/or `#undef _` at the top of your file.

## It's awesome and I totally want to use it, but how?

If you look up there ^ at that code snippet, it'll give you a basic idea of how things work right now.

You stick the library into your include folder, probably using something like `git submodule add https://github.com/toroidal-code/cppspec testing/include/cppspec`, and then import the main header, `cppspec.hpp`.

I'm (We're?) working on an actual test runner, instead of the weird boolean and-ing that you can see in the example, but the explicit `run`-ing works for now, and will probably always be an option.

There are really two ways to go about creating your tests: as a bunch of little individual executables, or as one giant monolithic one. CMake's CTest likes it when you have a bunch of individual test programs, but having a single runner file with a bunch of tests implemented in separate `something_spec.hpp` files might compile faster for you.

## Attribution
Heavily inspired by [RSpec](https://github.com/rspec) and [Jasmine](http://jasmine.github.io).

## Authors
Copyright 2016 Katherine Whitlock

## License
This code is licensed under the 3-clause BSD License.

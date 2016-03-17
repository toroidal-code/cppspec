#C++Spec
Have a taste of some RSpec in your C++.

```cpp
#include <cmath>
#include <cstdlib>
#include <list>
#include "cppspec.hpp"

int n = 0;
describe fabs_spec("fabs", _ {
  context("argument is zero", _ {
    it("return zero", _ {
      expect(fabs(n)).to_equal(n)();
    });
  });
    
  before("each", _ {
    n = rand();
  });
    
  context("argument is positive", _ {
    it("return positive", _ { 
      expect(fabs(n)).to_equal(n)
	    ("fabs(" + std::to_string(n) + ") didn't equal " + std::to_string(n));
    });
  });
    
  context("argument is negative", _ {
    it("return positive", _ {
      expect(fabs(-n)).to_equal(n)();
    });
  });
});

describe_a <std::list<int>> int_list_spec({1,2,3}, _ {
  it(_{ is_expected().to_include(3)(); });
});

int main(void) {
  bool r = true;
  r &= fabs_spec.run();
  r &= int_list.run();
  return r ? EXIT_SUCCESS : EXIT_FAILURE;
}

```

##Why?
Because BDD-testing on C and C++ sucks. It's hard. I wanted something where I could make comprehensive BDD tests really, _really_ easily.

##How?
Magic. From pixies. Unholy black C++ magic.

##No, really. How?
Lots of templates. Templates everywhere.

A single 'Describe' is actually a local variable that has a runtime-created execution tree. Calling `.run()` on the root recursively calls `.run()` on its' children.

A common expectation like `expect({1,2,3}).to_include({1,2});` is more like 
```cpp
Expecatations::Expectation<std::initializer_list<int>>({1,2,3}).to_include<std::initializer_list<int>>({1,2})
```

Not fun. Cool. But definitely not fun.

##It's Ugly!
Yeah, I know. Let's see you make it any prettier (and if you do, send me a pull request :P ).

In all honesty though, this was as close to RSpec or Jasmine syntax as I could get.

##It's Beautiful!
Why, thank you! I certainly tried.

##What the heck is `_`!?
It's literally `[](auto &self) -> auto`.  
We need this to pass a reference to the containing "thing". For example, `describe` blocks get a `Description` object, while `it` blocks get an `It` object.

## Attribution
Heavily inspired by [RSpec](https://github.com/rspec) and [Jasmine](http://jasmine.github.io).

## Authors
Copyright 2016 Katherine Whitlock

## License
This code is licensed under the 3-clause BSD License.

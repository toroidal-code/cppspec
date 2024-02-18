// Copyright 2016 Katherine Whitlock

#include "cppspec.hpp"
#include "formatters/verbose.hpp"

describe a_suite("A suite", $ {
  it("contains a spec with an expectation", _ {
    expect(true).to_be_true();
  });
});

describe suite_object("A suite is just an object wrapping a lambda", $ {
  bool a = false;

  it("and so is a spec", _ {
    a = true;
    expect(a).to_be_true();
  });
});

describe to_be_compare("The 'to_satisfy' matcher compares equality via evaluation",
                       $ {
  it("and has a positive case", _ {
    expect(true).to_satisfy([](const bool &b) { return b == true; });
  });

  it("and can have a negative case", _ {
    expect(false).not_().to_satisfy([](const bool &b) { return b == true; });
  });
});


describe to_equal_compare("The 'to_equal' matcher compares with '=='", $ {
  it("and has a positive case", _ {
    expect(true).to_equal(true);
  });

  it("and can have a negative case", _ {
    expect(false).not_().to_equal(true);
  });
});

struct AB { int a; int b; };
inline bool operator==(const AB& lhs, const AB&rhs) {
  return lhs.a == rhs.a && lhs.b == rhs.b;
}


describe included_matchers("Included matchers:", $ {
  it("The 'to_be' matcher compares equality via lambda evaluation", _ {
    int a = 12;
    int b = a;

    expect(a).to_satisfy([b](const int &value) -> bool { return value == b;});
    // to_equal is shorter for things that can be compared
    // with operator==
    expect(a).to_equal(b);
  });

  context("The 'to_equal' matcher", _ {
    it("works for simple literals and variables", _ {
      auto a = 12;
      expect(a).to_equal(12);
    });

    it("should work for objects with operator== defined", _ {
      AB foo;
      foo.a = 12;
      foo.b = 34;

      AB bar;
      bar.a = 12;
      bar.b = 34;

      expect(foo).to_equal(bar);
    });
  });
    // Not yet implemented
//    it("The 'to_match' matcher is for regular expressions", _ {
//      std::string message = "foo bar baz";
//
//      expect(message).to_match(std::regex("bar"));
//      expect(message).to_match("bar");
//      expect(message).not_().to_match(std::regex("quux"));
//    });

  it("The 'to_be_truthy' matcher is for boolean casting testing", _ {
    void* a = nullptr;
    //auto foo = "foo";

//    expect(foo).to_be_truthy();
    expect(a).not_().to_be_truthy();
  });

  it("The 'to_be_falsy' matcher is for boolean casting testing", _ {
    void* a = nullptr;
    //auto foo = "foo";

    expect(a).to_be_falsy();
//    expect(foo).not_().to_be_falsy();
  });

  it("The 'to_include' matcher is for finding an item in a container", _ {
    auto a = {"foo", "bar", "baz"};

    expect(a).to_contain("bar");
    expect(a).not_().to_contain("quux");
  });

  it("The 'to_be_less_than' matcher is for mathematical comparisons", _ {
    auto pi = 3.1415926,
        e = 2.78;

    expect(e).to_be_less_than(pi);
    expect(pi).not_().to_be_less_than(e);
  });

  it("The 'to_be_greater_than' matcher is for mathematical comparisons", _ {
    auto pi = 3.1415926,
        e = 2.78;

    expect(pi).to_be_greater_than(e);
    expect(e).not_().to_be_greater_than(pi);
  });

// TODO: take another look at Jasmine's toBeCloseTo
//    it("The 'to_be_within' matcher is for precision match comparison", _ {
//      auto pi = 3.1415926,
//          e = 2.78;
//
//      expect(pi).not_().to_be_within(0.5).of(e);
//    });

  it("The 'to_throw' matcher is for testing if a function throws an exception", _ {
    std::function<int()> foo = [] {
      return 1 + 2;
    };
    std::function<void*()> bar = [] {
      throw(std::exception());
      return nullptr;
    };

    expect(foo).not_().to_throw();
    expect(bar).to_throw();
  });
});


//describe fail_function_spec("A spec using the fail function", $ {
//  auto foo = [](auto &x, auto &callback) {
//    if (x) {
//      callback();
//    }
//  };
//
//  it("should not call the callback", _ {
//    foo(false, [] -> {
//      fail("Callback has been called");
//    });
//  });
//});

describe a_spec("A spec", $ {
  it("is just a function, so it can contain any code", _ {
    auto foo = 0;
    foo += 1;
    expect(foo).to_equal(1);
  });

  it("can have more than one expectation", _ {
    auto foo = 0;
    foo += 1;

    expect(foo).to_equal(1);
    expect(true).to_equal(true);
  });
});

describe a_spec_using_be_ae("A spec using before_each and after_each", $ {
  auto foo = 0;

  before_all([&foo]{ foo = 1; });
  after_all([&foo]{ foo = 0; });

  it("sets the initial value of foo before specs run", [&](auto &self) {
    expect(foo).to_equal(1);
    foo += 1;
  });

  it("does not reset foo between specs", _ {
    expect(foo).to_equal(2);
  });
});

describe a_spec_before_each("A spec", $ {
  int foo;
  before_each([&foo]{ foo = 0; });

  it("can use a variable to share state", _ {
    expect(foo).to_equal(0);
    foo += 1;
  });

  it("prevents test pollution by starting with a clean test environment", _ {
    expect(foo).to_equal(0);
  });
});

describe a_spec_nesting("A spec", $ {
  int foo;

  before_each([&foo] {
    foo = 0;
    foo += 1;
  });

  after_each([&foo] {
    foo = 0;
  });

  it("is just a function, so it can contain any code", _ {
    expect(foo).to_equal(1);
  });

  it("can have more than one expectation", _ {
    expect(foo).to_equal(1);
    expect(true).to_equal(true);
  });

  explain("nested contexts", _ {
    int bar;

    before_each([&bar] {
      bar = 1;
    });

    it("can reference both scopes as needed", _ {
      expect(foo).to_equal(bar);
    });
  });
});

// xdescribes are unnecessary, as test are simply not run or
// added to the Runner object.

// TODO: pending specs

auto to_include_matcher = describe("to contain", $ {
  let(foo, ([]() -> std::vector<int> { return {1,2,3,4}; }));

  it("matches arrays with some of the values", _ {
    expect(foo).to_contain({3,1});
    expect(foo).not_().to_contain(6);
  });
});

int main(int argc, char** argv) {
  return CppSpec::parse(argc, argv)
             .add_spec(a_suite)
             .add_spec(suite_object)
             .add_spec(to_be_compare)
             .add_spec(to_equal_compare)
             .add_spec(included_matchers)
             .add_spec(a_spec)
             .add_spec(a_spec_using_be_ae)
             .add_spec(a_spec_before_each)
             .add_spec(a_spec_nesting)
             .add_spec(to_include_matcher)
             .exec() ? EXIT_SUCCESS : EXIT_FAILURE;
}

#include <cmath>
#include <cstdlib>
#include <string>
#include <vector>

#include "cppspec.hpp"

using namespace CppSpec;

struct Counter {
  int value = 0;
  void increment() { value++; }
  void reset() { value = 0; }
  [[nodiscard]] bool is_zero() const { return value == 0; }
};

struct Point {
  double x, y;
  [[nodiscard]] double length() const { return std::sqrt(x * x + y * y); }
  bool operator==(const Point& o) const { return x == o.x && y == o.y; }
};

// describe_a with implicit subject (default constructor)
describe_a<Counter> counter_spec("Counter", $ {
  it("default constructs to zero", _ {
    expect(subject.value).to_equal(0);
    expect(subject.is_zero()).to_be_true();
  });

  it("increment increases value", _ {
    Counter c;
    c.increment();
    expect(c.value).to_equal(1);
    expect(c.is_zero()).to_be_false();
  });

  context("with a fresh counter for typed access", Counter{}, _ {
    it("starts at zero", _ {
      expect(subject.value).to_equal(0);
    });

    it("is_zero after reset is true", _ {
      subject.reset();
      expect(subject.is_zero()).to_be_true();
    });
  });
});

// describe_a with explicit subject
describe_a<Point> point_spec("Point{3,4}", Point{3.0, 4.0}, $ {
  it("has x == 3", _ {
    expect(subject.x).to_equal(3.0);
  });

  it("has y == 4", _ {
    expect(subject.y).to_equal(4.0);
  });

  it("has length 5 (3-4-5 triangle)", _ {
    expect(subject.length()).to_be_within(1e-9).of(5.0);
  });

  it("is_expected() refers to the subject", _ {
    is_expected().to_equal(Point{3.0, 4.0});
  });
});

// describe_a with initializer-list subject
describe_a<std::vector<int>> vec_spec({10, 20, 30, 40, 50}, $ {
  it("contains 30", _ {
    expect(subject).to_contain(30);
  });

  it("starts with {10, 20}", _ {
    expect(subject).to_start_with({10, 20});
  });

  it("ends with {40, 50}", _ {
    expect(subject).to_end_with({40, 50});
  });

  it("has size 5 via satisfy", _ {
    expect(subject).to_satisfy([](const std::vector<int>& x) { return x.size() == 5; });
  });

  it("is_expected() also works", _ {
    is_expected().to_contain(10);
  });
});

// describe_a with nested context: is_expected() and subject accessible
describe_a<std::string> inheritance_spec("string describe_a inheritance",
                                          std::string{"hello"}, $ {
  it("starts with hel", _ {
    expect(subject).to_start_with("hel");
  });

  it("is_expected() works in plain it", _ {
    is_expected().to_end_with("llo");
    is_expected().to_equal("hello");
  });

  context("nested plain context (same subject)", _ {
    it("can still use is_expected() from nested context", _ {
      is_expected().to_start_with("hel");
    });

    it("subject access also works", _ {
      expect(subject).to_equal("hello");
    });
  });
});

CPPSPEC_MAIN(counter_spec, point_spec, vec_spec, inheritance_spec);

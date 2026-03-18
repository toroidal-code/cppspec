#include <cmath>
#include <string>

#include "cppspec.hpp"

using namespace CppSpec;

describe satisfy_spec("to_satisfy matcher", $ {
  context("basic predicate", _ {
    it("passes when predicate returns true", _ {
      expect(42).to_satisfy([](int x) { return x > 0; });
    });

    it("fails when predicate returns false", _ {
      expect(-1).not_().to_satisfy([](int x) { return x > 0; });
    });

    it("works with equality predicate", _ {
      expect(7).to_satisfy([](int x) { return x == 7; });
    });

    it("works with modulo check", _ {
      expect(6).to_satisfy([](int x) { return x % 2 == 0; });
    });

    it("works with odd check negated", _ {
      expect(7).not_().to_satisfy([](int x) { return x % 2 == 0; });
    });
  });

  context("with floating point", _ {
    it("checks range via predicate", _ {
      expect(3.14).to_satisfy([](double x) { return x > 3.0 && x < 3.2; });
    });

    it("checks NaN", _ {
      expect(std::numeric_limits<double>::quiet_NaN())
          .to_satisfy([](double x) { return std::isnan(x); });
    });
  });

  context("with strings", _ {
    it("checks non-empty", _ {
      expect(std::string{"hello"})
          .to_satisfy([](const std::string& s) { return !s.empty(); });
    });

    it("checks prefix", _ {
      expect(std::string{"hello world"})
          .to_satisfy([](const std::string& s) { return s.substr(0, 5) == "hello"; });
    });

    it("negated: fails when predicate is true", _ {
      expect(std::string{""})
          .not_().to_satisfy([](const std::string& s) { return !s.empty(); });
    });
  });

  context("with booleans", _ {
    it("true satisfies identity predicate", _ {
      expect(true).to_satisfy([](bool b) { return b; });
    });

    it("false does not satisfy identity predicate", _ {
      expect(false).not_().to_satisfy([](bool b) { return b; });
    });
  });

  int threshold = 10;

  context("with captured variables", _ {
    it("captures threshold correctly", _ {
      expect(15).to_satisfy([threshold](int x) { return x > threshold; });
      expect(5).not_().to_satisfy([threshold](int x) { return x > threshold; });
    });
  });
});

CPPSPEC_MAIN(satisfy_spec);

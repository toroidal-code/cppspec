#include <expected>
#include <optional>

#include "cppspec.hpp"

using namespace CppSpec;

struct ParseError { std::string msg; };
struct NetworkError { int code; };

describe expected_spec("to_have_value and to_have_error matchers", $ {
  context("std::optional", _ {
    it("to_have_value passes when optional has a value", _ {
      std::optional<int> opt{42};
      expect(opt).to_have_value();
    });

    it("to_have_value fails when optional is empty", _ {
      std::optional<int> opt{};
      expect(opt).not_().to_have_value();
    });

    it("can access the value after to_have_value", _ {
      std::optional<std::string> opt{"hello"};
      expect(opt).to_have_value();
      expect(opt.value()).to_equal("hello");
    });

    it("optional<string> is empty", _ {
      std::optional<std::string> opt{};
      expect(opt).not_().to_have_value();
    });
  });

  context("std::expected with value", _ {
    it("to_have_value passes when expected contains a value", _ {
      std::expected<int, ParseError> e{42};
      expect(e).to_have_value();
    });

    it("not_().to_have_error passes when value present", _ {
      std::expected<int, ParseError> e{99};
      expect(e).not_().to_have_error();
    });

    it("to_have_value passes for string value", _ {
      std::expected<std::string, ParseError> e{"hello"};
      expect(e).to_have_value();
    });
  });

  context("std::expected with error", _ {
    it("to_have_error passes when expected contains an error", _ {
      std::expected<int, ParseError> e{std::unexpected(ParseError{"bad input"})};
      expect(e).to_have_error();
    });

    it("not_().to_have_error passes when value present", _ {
      std::expected<int, ParseError> e{7};
      expect(e).not_().to_have_error();
    });

    it("to_have_value fails when error present", _ {
      std::expected<int, NetworkError> e{std::unexpected(NetworkError{404})};
      expect(e).not_().to_have_value();
    });

    it("to_have_error passes for network error", _ {
      std::expected<std::string, NetworkError> e{std::unexpected(NetworkError{500})};
      expect(e).to_have_error();
    });
  });

  context("chaining with value checks", _ {
    it("can check value contents after confirming has_value", _ {
      std::expected<int, ParseError> e{123};
      expect(e).to_have_value();
      expect(e.value()).to_equal(123);
    });

    it("can check multiple expectations on same optional", _ {
      std::optional<double> opt{3.14};
      expect(opt).to_have_value();
      expect(opt.value()).to_be_greater_than(3.0);
      expect(opt.value()).to_be_less_than(4.0);
    });
  });
});

CPPSPEC_MAIN(expected_spec);

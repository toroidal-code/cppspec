#include <cstddef>
#include <cstdint>
#include <map>
#include <optional>
#include <ostream>
#include <set>
#include <source_location>
#include <string>
#include <vector>

#include "cppspec.hpp"

using namespace CppSpec;

enum class Colour : std::uint8_t { Red = 2, Blue = 3 };
enum class Letter : char { A = 'A' };
enum class Neg : std::int8_t { Value = -3 };
enum class Big : std::int64_t { Value = -5000000000 };
enum Plain { Seven = 7 };
enum class Streamed { X };

std::ostream& operator<<(std::ostream& os, Streamed /* value */) {
  return os << "Streamed::X!";
}

namespace {

// A range of bytes with an operator<< of its own.
struct Buf {
  std::vector<std::uint8_t> bytes;
  [[nodiscard]] auto begin() const { return bytes.begin(); }
  [[nodiscard]] auto end() const { return bytes.end(); }
};

std::ostream& operator<<(std::ostream& os, const Buf& buf) {
  return os << "Buf(" << buf.bytes.size() << " bytes)";
}

// A range of bytes with no operator<<.
struct Bytes {
  std::vector<std::uint8_t> bytes;
  [[nodiscard]] auto begin() const { return bytes.begin(); }
  [[nodiscard]] auto end() const { return bytes.end(); }
};

// The message Equal reports when it fails on the given values.
template <typename T>
std::string equal_failure(T actual, T expected) {
  auto ex = ExpectationValue(actual, std::source_location::current());
  return Matchers::Equal<T, T>(ex, expected).run().get_message();
}

}  // namespace

// clang-format off
describe pretty_spec("Pretty::to_word", $ {
  it("prints a scoped enum with no operator<< as its type and value", _ {
    expect(Pretty::to_word(Colour::Red)).to_equal(std::string("Colour(2)"));
  });

  it("prints a char-based enum with no operator<< as its type and number", _ {
    expect(Pretty::to_word(Letter::A)).to_equal(std::string("Letter(65)"));
  });

  it("prints a byte-sized integer as a number, not a character", _ {
    expect(Pretty::to_word(std::uint8_t{65})).to_equal(std::string("65"));
    expect(Pretty::to_word(std::int8_t{-3})).to_equal(std::string("-3"));
  });

  it("still prints a char as a character", _ {
    expect(Pretty::to_word('A')).to_equal(std::string("A"));
  });

  it("prints an empty optional as std::nullopt and a full one as its value", _ {
    expect(Pretty::to_word(std::optional<int>{})).to_equal(std::string("std::nullopt"));
    expect(Pretty::to_word(std::optional<int>{5})).to_equal(std::string("5"));
    expect(Pretty::to_word(std::optional<Colour>{Colour::Blue})).to_equal(std::string("Colour(3)"));
  });

  it("prints a negative enum value as a negative number", _ {
    expect(Pretty::to_word(Neg::Value)).to_equal(std::string("Neg(-3)"));
  });

  it("prints a 64-bit enum value in full", _ {
    expect(Pretty::to_word(Big::Value)).to_equal(std::string("Big(-5000000000)"));
  });

  it("prints an enum with an operator<< through that operator", _ {
    expect(Pretty::to_word(Streamed::X)).to_equal(std::string("Streamed::X!"));
  });

  it("prints a std::byte as its type and number", _ {
    expect(Pretty::to_word(std::byte{65})).to_equal(std::string("std::byte(65)"));
  });

  it("prints an unscoped enum as its number", _ {
    expect(Pretty::to_word(Seven)).to_equal(std::string("7"));
  });

  it("prints an optional holding an empty optional apart from an empty optional", _ {
    expect(Pretty::to_word(std::optional<std::optional<int>>{})).to_equal(std::string("std::nullopt"));
    expect(Pretty::to_word(std::optional<std::optional<int>>{std::optional<int>{}}))
        .to_equal(std::string("std::optional(std::nullopt)"));
    expect(Pretty::to_word(std::optional<std::optional<int>>{5})).to_equal(std::string("std::optional(5)"));
  });

  it("prints wide and Unicode characters as their code unit number", _ {
    expect(Pretty::to_word(L'a')).to_equal(std::string("97"));
    expect(Pretty::to_word(u8'a')).to_equal(std::string("97"));
    expect(Pretty::to_word(u'a')).to_equal(std::string("97"));
    expect(Pretty::to_word(U'a')).to_equal(std::string("97"));
    expect(L'a').to_equal(L'a');
    expect(u8'a').not_().to_equal(u8'b');
    expect(equal_failure(u'a', u'b')).to_partially_match("=> 98\n +got .* => 97");
  });

  it("prints a pointer to bytes or wide characters as an address, and a null one as nullptr", _ {
    const unsigned char bytes[] = {65, 66, 0};
    const signed char signed_bytes[] = {65, 0};
    const wchar_t wide[] = L"a";
    expect(Pretty::to_word(static_cast<const unsigned char*>(bytes))).to_match("0x[0-9a-f]+");
    expect(Pretty::to_word(static_cast<const signed char*>(signed_bytes))).to_match("0x[0-9a-f]+");
    expect(Pretty::to_word(static_cast<const wchar_t*>(wide))).to_match("0x[0-9a-f]+");
    expect(Pretty::to_word(static_cast<const unsigned char*>(nullptr))).to_equal(std::string("nullptr"));
    expect(Pretty::to_word(static_cast<signed char*>(nullptr))).to_equal(std::string("nullptr"));
    expect(Pretty::to_word(static_cast<const char16_t*>(nullptr))).to_equal(std::string("nullptr"));
  });

  context("inside a container", _ {
    it("prints byte-sized integers as numbers", _ {
      expect(Pretty::to_word(std::vector<std::uint8_t>{65, 66})).to_equal(std::string("[65, 66]"));
      expect(Pretty::to_word(std::vector<std::int8_t>{-3})).to_equal(std::string("[-3]"));
    });

    it("prints enums, optionals and null C strings as Pretty prints them alone", _ {
      expect(Pretty::to_word(std::vector<Colour>{Colour::Red})).to_equal(std::string("[Colour(2)]"));
      expect(Pretty::to_word(std::vector<std::optional<int>>{1, std::nullopt}))
          .to_equal(std::string("[1, std::nullopt]"));
      expect(Pretty::to_word(std::vector<const char*>{nullptr, "a"})).to_equal(std::string("[nullptr, a]"));
    });

    it("keeps the container's delimiters, and reaches nested containers and map entries", _ {
      expect(Pretty::to_word(std::set<std::uint8_t>{1, 2})).to_equal(std::string("{1, 2}"));
      expect(Pretty::to_word(std::vector<std::vector<std::uint8_t>>{{65}, {66}})).to_equal(std::string("[[65], [66]]"));
      expect(Pretty::to_word(std::map<int, std::uint8_t>{{1, 65}})).to_equal(std::string("[(1, 65)]"));
    });

    it("prints bools as true and false", _ {
      expect(Pretty::to_word(std::vector<bool>{true, false})).to_equal(std::string("[true, false]"));
    });

    it("prints an empty container as its delimiters alone", _ {
      expect(Pretty::to_word(std::vector<int>{})).to_equal(std::string("[]"));
      expect(Pretty::to_word(std::vector<std::uint8_t>{})).to_equal(std::string("[]"));
      expect(Pretty::to_word(std::set<char>{})).to_equal(std::string("{}"));
    });

    it("prints any other container as before", _ {
      expect(Pretty::to_word(std::vector<int>{1, 2})).to_equal(std::string("[1, 2]"));
      expect(Pretty::to_word(std::set<char>{'a'})).to_equal(std::string("{a}"));
    });

    it("prints a range type with its own operator<< through that operator", _ {
      expect(Pretty::to_word(Buf{{65, 66}})).to_equal(std::string("Buf(2 bytes)"));
      expect(Pretty::to_word(std::vector<Buf>{Buf{{65, 66}}})).to_equal(std::string("[Buf(2 bytes)]"));
    });

    it("prints a range type with no operator<< element by element", _ {
      expect(Pretty::to_word(Bytes{{65, 66}})).to_equal(std::string("[65, 66]"));
    });

    it("prints two byte buffers as numbers when Equal fails", _ {
      expect(equal_failure(std::vector<std::uint8_t>{65, 66}, std::vector<std::uint8_t>{65, 67}))
          .to_partially_match("=> \\[65, 67\\]\n +got .* => \\[65, 66\\]");
    });

    it("compares containers of enums and of optionals, printing them by value", _ {
      expect(std::vector<Colour>{Colour::Red}).to_equal(std::vector<Colour>{Colour::Red});
      expect(std::vector<std::optional<int>>{1}).not_().to_equal(std::vector<std::optional<int>>{std::nullopt});
      expect(equal_failure(std::vector<Colour>{Colour::Red}, std::vector<Colour>{Colour::Blue}))
          .to_partially_match("=> \\[Colour\\(3\\)\\]\n +got .* => \\[Colour\\(2\\)\\]");
      expect(equal_failure(std::vector<std::optional<int>>{1}, std::vector<std::optional<int>>{std::nullopt}))
          .to_partially_match("=> \\[std::nullopt\\]\n +got .* => \\[1\\]");
    });
  });

  it("strips MSVC's enum, class, struct and union keywords from a type name, wherever they start a word", _ {
    expect(Util::strip_type_keywords("enum Colour")).to_equal(std::string("Colour"));
    expect(Util::strip_type_keywords("class std::vector<enum Colour,class std::allocator<enum Colour> >"))
        .to_equal(std::string("std::vector<Colour,std::allocator<Colour> >"));
    expect(Util::strip_type_keywords("struct `anonymous namespace'::Buf"))
        .to_equal(std::string("`anonymous namespace'::Buf"));
    expect(Util::strip_type_keywords("union U")).to_equal(std::string("U"));
    expect(Util::strip_type_keywords("my_class Foo")).to_equal(std::string("my_class Foo"));
    expect(Util::strip_type_keywords("Colour")).to_equal(std::string("Colour"));
  });

  it("takes the last piece of a name, or nothing from a name with no pieces", _ {
    expect(Pretty::last("CppSpec::Matchers::Equal", ':')).to_equal(std::string("Equal"));
    expect(Pretty::last("", ':')).to_equal(std::string());
    expect(Pretty::last("::", ':')).to_equal(std::string());
  });

  it("prints both enum values when Equal fails", _ {
    auto ex = ExpectationValue(Colour::Red, std::source_location::current());
    expect(Matchers::Equal<Colour, Colour>(ex, Colour::Blue).run().get_message())
        .to_equal(std::string("expected (Colour) => Colour(3)\n     got (Colour) => Colour(2)\nCompared using `==`"));
  });
});

CPPSPEC_MAIN(pretty_spec);

#include <cstdint>
#include <optional>
#include <source_location>
#include <string>

#include "cppspec.hpp"

using namespace CppSpec;

enum class Colour : std::uint8_t { Red = 2, Blue = 3 };
enum class Letter : char { A = 'A' };

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

  it("prints both enum values when Equal fails", _ {
    auto ex = ExpectationValue(Colour::Red, std::source_location::current());
    expect(Matchers::Equal<Colour, Colour>(ex, Colour::Blue).run().get_message())
        .to_equal(std::string("expected (Colour) => Colour(3)\n     got (Colour) => Colour(2)\nCompared using `==`"));
  });
});

CPPSPEC_MAIN(pretty_spec);

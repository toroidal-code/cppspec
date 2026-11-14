#include <optional>
#include <source_location>
#include <string>
#include <string_view>

#include "cppspec.hpp"

using namespace CppSpec;

namespace {

const char* const null_text = nullptr;

// The line a search matcher adds below its message when the actual value is a null C string.
const std::string null_text_note = "\nnullptr is not text, so there is nothing to search";

// The Result a matcher reports, outside any example.
template <typename Matcher>
Result outcome(Matcher matcher) {
  return matcher.run();
}

// An expectation on a value that is negated before any matcher sees it.
template <typename T>
ExpectationValue<T> negated(T value) {
  ExpectationValue<T> expectation(value, std::source_location::current());
  expectation.not_();
  return expectation;
}

}  // namespace

// clang-format off
describe c_string_spec("expect on a C string", $ {
  context("as a pointer", _ {
    it("passes to_be_null on a null C string", _ {
      const char* s = nullptr;
      expect(s).to_be_null();
    });

    it("passes not_().to_be_null on a non-null C string", _ {
      const char* s = "abc";
      expect(s).not_().to_be_null();
    });

    it("keeps a volatile char pointer as a plain pointer", _ {
      static_assert(!Util::is_c_string<volatile char*>);
      static_assert(!Util::is_c_string<const volatile char*>);
      volatile char* none = nullptr;
      volatile char c = 'a';
      volatile char* some = &c;
      expect(none).to_be_null();
      expect(some).not_().to_be_null();
    });
  });

  context("compared as a string", _ {
    it("equals a std::string with the same text", _ {
      std::string owner("abc");
      const char* s = owner.c_str();
      expect(s).to_equal(std::string("abc"));
      expect(s).not_().to_equal(std::string("abd"));
      expect(std::string("abc")).to_equal(s);
    });

    it("equals another C string with the same text at another address", _ {
      std::string owner("abc");
      const char* s = owner.c_str();
      expect(s).to_equal("abc");
      expect(s).not_().to_equal("abd");
    });

    it("compares a char array and a char pointer as strings", _ {
      char text[] = "abc";
      char* s = text;
      expect(text).to_equal("abc");
      expect(std::string("abc")).to_equal(s);
    });

    it("starts and ends with text", _ {
      std::string owner("abc");
      const char* s = owner.c_str();
      expect(s).to_start_with("ab");
      expect(s).not_().to_start_with("bc");
      expect(s).to_end_with("bc");
      expect(s).not_().to_end_with("ab");
    });

    it("matches and partially matches a pattern", _ {
      const char* s = "abc";
      expect(s).to_match("a.c");
      expect(s).not_().to_match("b");
      expect(s).to_partially_match("b");
      expect(s).not_().to_partially_match("x");
    });

    it("contains text and characters", _ {
      const char* s = "abc";
      expect(s).to_contain("bc");
      expect(s).not_().to_contain("ca");
      expect(s).to_contain('b');
      expect(s).not_().to_contain('x');
    });

    it("lets a std::string and a std::string_view start, end with and contain text", _ {
      expect(std::string_view("abc")).to_start_with("ab");
      expect(std::string_view("abc")).to_end_with("bc");
      expect(std::string("abc")).to_contain("bc");
      expect(std::string("abc")).to_contain(std::string("ab"));
    });
  });

  context("when null", _ {
    it("equals another null C string", _ {
      const char* other = nullptr;
      expect(null_text).to_equal(other);
    });

    it("differs from every string", _ {
      expect(null_text).not_().to_equal("abc");
      expect(null_text).not_().to_equal("");
      expect(null_text).not_().to_equal(std::string("abc"));
      expect("abc").not_().to_equal(null_text);
      expect(std::string("abc")).not_().to_equal(null_text);
    });

    it("fails Equal against a string, printing nullptr and the string", _ {
      auto ex = ExpectationValue(null_text, std::source_location::current());
      Result r = outcome(Matchers::Equal<const char*, const char*>(ex, "abc"));
      expect(r).to_fail();
      expect(r.get_message())
          .to_partially_match("expected \\(const char \\*\\) => \"abc\"\n +got \\(const char \\*\\) => nullptr");
    });

    it("fails Equal against a null expected C string", _ {
      auto ex = ExpectationValue(std::string("abc"), std::source_location::current());
      expect(outcome(Matchers::Equal<std::string, const char*>(ex, null_text))).to_fail();
    });

    it("fails negated Equal against another null C string", _ {
      auto nex = negated(null_text);
      Result r = outcome(Matchers::Equal<const char*, const char*>(nex, null_text));
      expect(r).to_fail();
      expect(r.get_message()).to_partially_match("expected not \\(const char \\*\\) => nullptr");
    });

    it("fails start_with and end_with either way, saying nullptr has no text", _ {
      auto ex = ExpectationValue(null_text, std::source_location::current());
      auto nex = negated(null_text);
      for (const Result& r : {outcome(Matchers::StartWith<const char*, std::string>(ex, "ab")),
                              outcome(Matchers::StartWith<const char*, std::string>(nex, "ab")),
                              outcome(Matchers::EndWith<const char*, std::string>(ex, "bc")),
                              outcome(Matchers::EndWith<const char*, std::string>(nex, "bc"))}) {
        expect(r).to_fail();
        expect(r.get_message()).to_partially_match("expected nullptr to");
        expect(r.get_message()).to_partially_match(null_text_note);
      }
    });

    it("fails match and partially_match either way, saying nullptr has no text", _ {
      auto ex = ExpectationValue(null_text, std::source_location::current());
      auto nex = negated(null_text);
      for (const Result& r : {outcome(Matchers::Match<const char*>(ex, std::string("a"))),
                              outcome(Matchers::Match<const char*>(nex, std::string("a"))),
                              outcome(Matchers::MatchPartial<const char*>(ex, std::string("a"))),
                              outcome(Matchers::MatchPartial<const char*>(nex, std::string("a")))}) {
        expect(r).to_fail();
        expect(r.get_message()).to_partially_match("expected nullptr to");
        expect(r.get_message()).to_partially_match(null_text_note);
      }
    });

    it("fails contain either way, saying nullptr has no text", _ {
      auto ex = ExpectationValue(null_text, std::source_location::current());
      auto nex = negated(null_text);
      for (const Result& r : {outcome(Matchers::Contain<const char*, const char*, const char*>(ex, "b")),
                              outcome(Matchers::Contain<const char*, const char*, const char*>(nex, "b")),
                              outcome(Matchers::Contain<const char*, char, char>(ex, 'b')),
                              outcome(Matchers::Contain<const char*, char, char>(nex, 'b'))}) {
        expect(r).to_fail();
        expect(r.get_message()).to_partially_match("expected nullptr to");
        expect(r.get_message()).to_partially_match(null_text_note);
      }
    });
  });

  context("printed", _ {
    it("prints a null C string as nullptr", _ {
      expect(Pretty::to_word(null_text)).to_equal(std::string("nullptr"));
      expect(Pretty::inspect_object(null_text)).to_equal(std::string("(const char *) => nullptr"));
    });

    it("prints a C string as its quoted text", _ {
      char text[] = "abc";
      char* s = text;
      expect(Pretty::to_word(s)).to_equal(std::string("abc"));
      expect(Pretty::inspect_object(static_cast<const char*>(s))).to_equal(std::string("(const char *) => \"abc\""));
      expect(Pretty::inspect_object(s)).to_equal(std::string("(char *) => \"abc\""));
    });
  });

  context("string_view_of", _ {
    it("views a std::string, a std::string_view, a char array and a char pointer", _ {
      const std::string owned("abc");
      const std::string_view view("abc");
      char text[] = "abc";
      char* pointer = text;
      const std::optional<std::string_view> abc("abc");
      expect(string_view_of(owned)).to_equal(abc);
      expect(string_view_of(view)).to_equal(abc);
      expect(string_view_of(text)).to_equal(abc);
      expect(string_view_of(pointer)).to_equal(abc);
      expect(string_view_of(static_cast<const char*>(pointer))).to_equal(abc);
    });

    it("views nothing for a null C string", _ {
      expect(string_view_of(null_text)).to_equal(std::optional<std::string_view>{});
    });

    it("reads a char array up to its first NUL and no further than its end", _ {
      const char padded[8] = "ab";
      const char unterminated[3] = {'a', 'b', 'c'};
      expect(string_view_of(padded)).to_equal(std::optional<std::string_view>("ab"));
      expect(string_view_of(unterminated)).to_equal(std::optional<std::string_view>("abc"));
    });
  });
});

CPPSPEC_MAIN(c_string_spec);

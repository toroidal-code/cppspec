#include <regex>
#include <string>
#include <vector>

#include "cppspec.hpp"

using namespace CppSpec;

describe string_spec("string matchers", $ {
  context("to_start_with (string)", _ {
    it("passes when string starts with prefix", _ {
      expect(std::string{"hello world"}).to_start_with("hello");
    });

    it("passes when prefix equals the full string", _ {
      expect(std::string{"hi"}).to_start_with("hi");
    });

    it("fails when string does not start with prefix", _ {
      expect(std::string{"hello"}).not_().to_start_with("world");
    });

    it("fails on partial mismatch at start", _ {
      expect(std::string{"goodbye"}).not_().to_start_with("hello");
    });

    it("passes with empty prefix", _ {
      expect(std::string{"anything"}).to_start_with("");
    });
  });

  context("to_end_with (string)", _ {
    it("passes when string ends with suffix", _ {
      expect(std::string{"hello world"}).to_end_with("world");
    });

    it("passes when suffix equals the full string", _ {
      expect(std::string{"hi"}).to_end_with("hi");
    });

    it("fails when string does not end with suffix", _ {
      expect(std::string{"hello"}).not_().to_end_with("world");
    });

    it("fails on partial suffix mismatch", _ {
      expect(std::string{"foobar"}).not_().to_end_with("baz");
    });

    it("passes with empty suffix", _ {
      expect(std::string{"anything"}).to_end_with("");
    });
  });

  context("to_start_with (container sequences)", _ {
    it("vector starts with sub-sequence", _ {
      expect(std::vector<int>{1, 2, 3, 4, 5}).to_start_with({1, 2, 3});
    });

    it("fails when sub-sequence does not match start", _ {
      expect(std::vector<int>{1, 2, 3}).not_().to_start_with({2, 3});
    });

    it("passes for single-element prefix", _ {
      expect(std::vector<int>{7, 8, 9}).to_start_with({7});
    });
  });

  context("to_end_with (container sequences)", _ {
    it("vector ends with sub-sequence", _ {
      expect(std::vector<int>{1, 2, 3, 4, 5}).to_end_with({3, 4, 5});
    });

    it("fails when sub-sequence does not match end", _ {
      expect(std::vector<int>{1, 2, 3}).not_().to_end_with({1, 2});
    });

    it("passes for single-element suffix", _ {
      expect(std::vector<int>{7, 8, 9}).to_end_with({9});
    });
  });

  context("to_match (full regex)", _ {
    it("matches an exact string pattern", _ {
      expect(std::string{"hello"}).to_match("hello");
    });

    it("matches with wildcard pattern", _ {
      expect(std::string{"hello"}).to_match("hel.*");
    });

    it("matches digit pattern", _ {
      expect(std::string{"12345"}).to_match("[0-9]+");
    });

    it("fails when pattern does not match", _ {
      expect(std::string{"hello"}).not_().to_match("world");
    });

    it("fails when partial match only (regex_match is full)", _ {
      expect(std::string{"hello"}).not_().to_match("hel");
    });

    it("matches case-insensitively with flag", _ {
      expect(std::string{"HELLO"}).to_match(std::regex("hello", std::regex::icase));
    });
  });

  context("string to_contain (char)", _ {
    it("finds a character in a string", _ {
      expect(std::string{"abcde"}).to_contain('c');
    });

    it("fails when char absent", _ {
      expect(std::string{"abcde"}).not_().to_contain('z');
    });
  });
});

CPPSPEC_MAIN(string_spec);

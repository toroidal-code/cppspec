#include <list>
#include <set>
#include <source_location>
#include <span>
#include <string>
#include <vector>

#include "cppspec.hpp"

using namespace CppSpec;

namespace {

// The results an expectation on actual reports to an example of its own when search runs it, positive and then
// negated.
template <typename A, typename Search>
std::list<Result> searched_both_ways(A actual, Search search) {
  auto location = std::source_location::current();
  ItD example(location, "searches", [](ItD&) {});
  ExpectationValue<A> positive(example, actual, location);
  search(positive);
  ExpectationValue<A> negative(example, actual, location);
  negative.not_();
  search(negative);
  return example.get_results();
}

// Whether a search failed positive and passed negated.
bool fails_and_passes_negated(const std::list<Result>& results) {
  return results.size() == 2 && results.front().is_failure() && results.back().is_success();
}

}  // namespace

// clang-format off
describe contain_spec("to_contain matcher", $ {
  context("with a single element", _ {
    it("passes when element is in vector", _ {
      expect(std::vector<int>{1, 2, 3}).to_contain(2);
    });

    it("fails when element is not in vector", _ {
      expect(std::vector<int>{1, 2, 3}).not_().to_contain(9);
    });

    it("passes when element is in list", _ {
      expect(std::list<int>{10, 20, 30}).to_contain(20);
    });

    it("fails when element is not in list", _ {
      expect(std::list<int>{10, 20, 30}).not_().to_contain(99);
    });

    it("passes for a string containing a char", _ {
      expect(std::string{"hello"}).to_contain('e');
    });

    it("fails when char not in string", _ {
      expect(std::string{"hello"}).not_().to_contain('z');
    });
  });

  context("with multiple elements (initializer list)", _ {
    it("passes when all elements are present", _ {
      expect(std::vector<int>{1, 2, 3, 4, 5}).to_contain({1, 3, 5});
    });

    it("not_().to_contain passes when none of the elements are present", _ {
      expect(std::vector<int>{1, 2, 3}).not_().to_contain({7, 9});
    });

    it("passes for a single-element list", _ {
      expect(std::vector<int>{5, 6, 7}).to_contain({6});
    });
  });

  context("with initializer-list actual", _ {
    it("passes when element found", _ {
      expect({10, 20, 30}).to_contain(20);
    });

    it("passes for subset", _ {
      expect({1, 2, 3, 4}).to_contain({2, 4});
    });

    it("fails when subset missing", _ {
      expect({1, 2, 3}).not_().to_contain({4, 5});
    });
  });

  context("negation", _ {
    it("not_().to_contain passes when all items absent", _ {
      expect(std::vector<int>{1, 2, 3}).not_().to_contain({7, 8, 9});
    });

    it("passes not_() when no items found", _ {
      expect(std::vector<int>{1, 2, 3}).not_().to_contain({4, 5, 6});
    });
  });

  context("with strings", _ {
    it("checks if string vector contains a string", _ {
      expect(std::vector<std::string>{"foo", "bar", "baz"}).to_contain(std::string{"bar"});
    });

    it("negated passes when string absent", _ {
      expect(std::vector<std::string>{"foo", "bar"}).not_().to_contain(std::string{"quux"});
    });
  });

  context("on an empty collection", _ {
    it("fails, and passes negated, without reading an element", _ {
      const std::vector<int> none;
      const std::vector<std::string> no_words;
      expect(fails_and_passes_negated(searched_both_ways(none, [](auto& e) { e.to_contain(1); }))).to_be_true();
      expect(fails_and_passes_negated(searched_both_ways(none, [](auto& e) { e.to_contain({1, 2}); }))).to_be_true();
      expect(fails_and_passes_negated(searched_both_ways(std::span<const int>{}, [](auto& e) { e.to_contain(1); })))
          .to_be_true();
      expect(fails_and_passes_negated(searched_both_ways(std::list<int>{}, [](auto& e) { e.to_contain(1); })))
          .to_be_true();
      expect(fails_and_passes_negated(searched_both_ways(no_words, [](auto& e) { e.to_contain(std::string{"foo"}); })))
          .to_be_true();
    });

    it("prints the empty collection when it fails", _ {
      const std::list<Result> results = searched_both_ways(std::vector<int>{}, [](auto& e) { e.to_contain(1); });
      expect(results.front().get_message()).to_partially_match("expected \\[\\] to contain 1");
    });
  });

  context("on empty text", _ {
    it("fails, and passes negated, for a character or text to look for", _ {
      const std::string empty;
      const char* const empty_c_string = "";
      expect(fails_and_passes_negated(searched_both_ways(empty, [](auto& e) { e.to_contain('a'); }))).to_be_true();
      expect(fails_and_passes_negated(searched_both_ways(empty, [](auto& e) { e.to_contain("a"); }))).to_be_true();
      expect(fails_and_passes_negated(searched_both_ways(empty_c_string, [](auto& e) { e.to_contain('a'); })))
          .to_be_true();
      expect(fails_and_passes_negated(searched_both_ways(empty_c_string, [](auto& e) { e.to_contain("a"); })))
          .to_be_true();
    });

    it("contains empty text", _ {
      expect(std::string{}).to_contain(std::string{});
      expect("").to_contain("");
    });
  });
});

CPPSPEC_MAIN(contain_spec);

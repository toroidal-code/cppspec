#include <list>
#include <set>
#include <vector>

#include "cppspec.hpp"

using namespace CppSpec;

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
});

CPPSPEC_MAIN(contain_spec);

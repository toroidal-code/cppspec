#include <cmath>
#include <cstdlib>
#include <list>
#include "cppspec.hpp"

describe bool_spec("Some Tests", $ {
  context("true is", _ {
    it("true", _ {
      expect(static_cast<bool>(1)).to_equal(true);
    });

    it("true", _ {
      expect(true).to_equal(true);
    });
  });

  int i = 4;
  explain("4", _ {
    it("equals 4", _ {
      expect(i).to_equal(4);
    });
    it("does not equal 5", _ {
      expect(i).not_().to_equal(5);
    });

    it("plus 1 equals 5", _ {
      expect(i+1).to_equal(5);
    });
  });

  explain("0 is", _ {
    using namespace CppSpec::Matchers;
    it("between -1 and 1 (exclusive)", _ {
      expect(0).to_be_between(-1, 1, RangeMode::exclusive);
    });
    it("between -0 and 0 (inclusive)", _ {
      expect(0).to_be_between(-0,0, RangeMode::inclusive);
    });
    it("not between -0 and 0 (exclusive)", _ {
      expect(0).not_().to_be_between(-0,0, RangeMode::exclusive);
    });
    it("not between 1 and -1", _ {
      expect(0).not_().to_be_between(1,-1);
    });
    it("between -1.0 and 1.0 (exclusive)", _ {
      expect(0).to_be_between(-1.0, 1.0, RangeMode::exclusive);
    });
  });


  explain("{1,2,3}", _ {
    it("includes 1", _ {
      expect({1,2,3}).to_include({1});
    });
    it("includes [1,2,3]", _ {
      expect({1,2,3}).to_include({1,2,8});
      expect({1,2,3}).to_include({1,2,7});
    });
    it("does not include 4", _ {
      expect({1,2,3}).not_().to_include({4});
    });

    it("does not include [4,5,6]", _ {
      expect({1,2,3}).not_().to_include({4,5,6});
    });
  });


  explain <std::list<int>> ({1,2,3}, _ {
    it(_ { is_expected().to_include(1); });

    it("includes [1,2,3]", _ {
      expect<std::list<int>>({1,2,3}).to_include({1,2,3});
    });

    it( _ { is_expected().not_().to_include(4); });

    it("does not include [4,5,6]", _ {
      is_expected().not_().to_include({4,5,6});
    });
  });

  // context("std::map {{1,2},{3,4},{5,6}}", _ {
  //     it("includes 1", _ {
  //         std::map<int,int> m = {{1,2},{3,4},{5,6}};
  //         expect(m).to_include({1});
  //       });
  //     it("includes [1,2,3]", _ {
  //         std::map<int,int> m = {{1,2},{3,4},{5,6}};
  //         expect(m).to_include({1,2,3});
  //       });
  //     it("does not include 4", _ {
  //         std::map<int,int> m = {{1,2},{3,4},{5,6}};
  //         expect(m).not_().to_include({4});
  //       });
  //     it("does not include 4,5,6", _ {
  //         std::map<int,int> m = {{1,2},{3,4},{5,6}};
  //         expect({1,2,3}).not_().to_include({4,5,6});
  //       });
  //   });
});

describe abs_spec("abs", $ {
  // you can use the `explain` keyword to
  // group behavior and nest descriptions
  explain("argument is zero", _ {
    it("return zero", _ {
      expect(abs(0)).to_equal(0);
    });
  });

  before_all([]{ std::srand(std::time(0)); });

  int n = 0;
  before_each([&]{ n = std::rand(); });

  // you can also use `context` instead of
  // `explain`, just like in RSpec
  context("argument is positive", _ {
    it("return positive", _ {
      expect(abs(n)).to_equal(n, "abs(" + std::to_string(n) + ") didn't equal " + std::to_string(n));
    });
  });

  explain("argument is negative", _ {
    it("return positive", _ {
      expect(abs(-n)).to_equal(n);
    });
  });
});


/* Here is the description of strcmp */
describe strcmp_spec("int strcmp ( const char * str1, const char * str2 )", $ {
  auto greater_than_zero = [](int i){return i>=0;};
  auto less_than_zero = [](int i){return i<0;};

  it("returns 0 only when strings are equal", _ {
    expect(strcmp("hello", "hello")).to_equal(0);
  });

  it("returns a negative integer when str1 is less than str2", _ {
    expect(strcmp("hello", "world")).to_be(less_than_zero);
    expect(strcmp("0123", "1321431")).to_be(less_than_zero);
  });

  it("returns a positive integer if str1 is greater than str2", _ {
    expect(strcmp("yellow", "world")).to_be(greater_than_zero);
    expect(strcmp("9", "789")).to_be(greater_than_zero);
  });
});

describe_a <std::vector<int>> vector_spec({1,2,3,4}, $ {
  it("should contain 2", _ {
    expect({1,2,3,4}).to_include(2);
  });

  it( _ { is_expected().to_include(2); });
});

int _count = 0;
describe let_spec("let", $ {
  let(count, [&]{ return ++_count ;});

  it("memoizes the value", _ {
    expect(count).to_equal(1);
    expect(count).to_equal(1);
  });

  it("is not cached across examples", _ {
    expect(count).to_equal(2);
  });
});


// describe_a<std::vector<int>> another_vector_spec({1,2,3,4}, _ {
//     // it(_ { assert(static_cast<ClassDescription<std::vector<int>>*>(self.get_parent()) != nullptr); });
//     it(_ {
//         ClassDescription<T> cd =
//       static_cast<ClassDescription<T> >(*(this->get_parent()));
//         self.is_expected(cd);
//  });
//   });

describe list_spec("A list spec", $ {
  explain <std::list<int>> ({1,2,3,4}, _ {
    it( _ { is_expected().to_include(8); });
  });
});

/* Here is the declaration of fabs description defined in an other file (fabs_spec.c in this sample)*/
int main(){
  return CppSpec::Runner(CppSpec::Formatters::verbose)
             .add_spec(bool_spec)
             .add_spec(abs_spec)
             .add_spec(strcmp_spec)
             .add_spec(vector_spec)
             .add_spec(let_spec)
             .add_spec(list_spec)
             .exec() ? EXIT_SUCCESS : EXIT_FAILURE;
}

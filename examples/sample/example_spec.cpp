#include <list>
#include <cassert>
#include "cppspec.hpp"

describe bool_spec("Some Tests", _ {
    // context("true is", _ {
    //     it("true", _ {
    //         expect(true).to_be;
    //       });
    //   });

    context("4 is", _ {
        it("equals 4", _ {
            expect(4).to_equal(4);
          });
        it("does not equal 5", _ {
            expect(4).not_().to_equal(5);
          });
      });

    context("0 is", _ {
        it("between -1 and 1 (exclusive)", _ {
            expect(0).to_be_between(-1, 1, Matchers::RangeMode::exclusive);
          });
        it("between -0 and 0 (inclusive)", _ {
            expect(0).to_be_between(-0,0, Matchers::RangeMode::inclusive);
          });
        it("not between -0 and 0 (exclusive)", _ {
            expect(0).not_().to_be_between(-0,0, Matchers::RangeMode::exclusive);
          });
        it("not between 1 and -1", _ {
            expect(0).not_().to_be_between(1,-1);
          });
        it("between -1.0 and 1.0 (exclusive)", _ {
            expect(0).to_be_between(-1.0, 1.0, Matchers::RangeMode::exclusive);
          });
      });


    context("{1,2,3}", _ {
        it("includes 1", _ {
            expect({1,2,3}).to_include({1});
          });
        it("includes [1,2,3]", _ {
            expect({1,2,3}).to_include({1,2,3});
          });
        it("does not include 4", _ {
            expect({1,2,3}).not_().to_include({4});
          });

        it("does not include [4,5,6]", _ {
            expect({1,2,3}).not_().to_include({4,5,6});
          });
      });


    context <std::list<int>>({1,2,3}, _ {
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

    // before("each", _ {
    //     n = rand();
    //   });

    // context("argument is positive", _ {
    //     it("return positive", _ {
    //         expect_to_equal(fabs(n), n);
    //       });
    //   });

    // context("argument is negative", _ {
    //     it("return positive", _ {
    //         expect(fabs(-n).to(equal(n));
    //       });
    //   });
  });

/* Here is the description of strcmp */
// describe strcmp_spec("int strcmp ( const char * str1, const char * str2 )", _ {
//   it( "returns 0 only when strings are equal", _ {
//       // should_equal( strcmp("hello", "hello"), 0);
//       // should_not_be_null( strcmp("hello", "world") );
//   });

//   it( "returns a negative integer when str1 is less than str2", _ {
//       // should_be_true( strcmp("hello", "world") < 0 );
//       // should_be_true( strcmp("0123", "1321431") < 0 );
//   });

//   it( "returns a positive integer if str1 is greater than str2", _ {
//       // should_be_true( strcmp("yellow", "world") > 0 );
//       // should_be_true( strcmp("9", "789") > 0 );
//   });
// });

describe_a <std::vector<int>> vector_spec({1,2,3,4}, _ {
    it("should contain 2", _ {
        expect({1,2,3,4}).to_include(6);
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

describe_a <std::vector<int>> another_vector_spec({1,2,3,4}, _ {
    it( _ { is_expected().to_include(6); });
  });


describe list_spec("A list spec", _ {
    context<std::list<int>>({1,2,3,4}, _ {
        it( _ { is_expected().to_include(6); });
      });
  });

/* Here is the declaration of fabs description defined in an other file (fabs_spec.c in this sample)*/
int main(){
  bool r = true;
  r &= bool_spec.run();
  r &= vector_spec.run();
  r &= another_vector_spec.run();
// #undef expect
// #undef it
//  Description d("halp", _ {});
//  It i =  d.it("me", _ {});
//  auto expectation =  i.expect(2);
//  auto be_between = expectation.to_be_between(1,3);
//  be_between();
  //strcmp_spec.run();
  return r ? EXIT_SUCCESS : EXIT_FAILURE;
}

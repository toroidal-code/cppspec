/* This sample file will describe the 2 following functions:
 *  . fabs
 *  . strcmp
*/

#include <list>
#include "cppspec.hpp"

describe bool_spec("Some Tests", _blk {
    // context("true is", _blk {
    //     it("true", _blk {
    //         expect(true).to_be;
    //       });
    //   });

    context("4 is", _blk {
        it("equals 4", _blk {
            expect(4).to_equal(4)();
          });
        it("does not equal 5", _blk {
            expect(4).not_().to_equal(5)();
          });
      });

    context("0 is", _blk {
        it("between -1 and 1 (exclusive)", _blk {
            expect(0).to_be_between(-1, 1).exclusive()();
          });
        it("between -0 and 0 (inclusive)", _blk {
            expect(0).to_be_between(-0,0).inclusive()();
          });
        it("not between -0 and 0 (exclusive)", _blk {
            expect(0).not_().to_be_between(-0,0).exclusive()();
          });
        it("not between 1 and -1", _blk {
            expect(0).not_().to_be_between(1,-1).exclusive()();
          });
        it("between -1.0 and 1.0 (exclusive)", _blk {
            expect(0).to_be_between(-1.0, 1.0).exclusive()();
          });
      });


    context("{1,2,3}", _blk {
        it("includes 1", _blk {
            expect({1,2,3}).to_include({1})();
          });
        it("includes [1,2,3]", _blk {
            expect({1,2,3}).to_include({1,2,3})();
          });
        it("does not include 4", _blk {
            expect({1,2,3}).not_().to_include({4})();
          });

        it("does not include [4,5,6]", _blk {
            expect({1,2,3}).not_().to_include({4,5,6})();
          });
      });


    context("std::list {1,2,3}", _blk {
        it("includes 1", _blk {
            std::list<int> l = {1,2,3};
            expect(l).to_include({1})();
          });
        it("includes [1,2,3]", _blk {
            std::list<int> l = {1,2,3};
            expect(l).to_include({1,2,3})();
          });
        it("does not include 4", _blk {
            std::list<int> l = {1,2,3};
            expect(l).not_().to_include({4})();
          });
        it("does not include [4,5,6]", _blk {
            std::list<int> l = {1,2,3};
            expect(l).not_().to_include({4,5,6})();
          });
      });

    // context("std::map {{1,2},{3,4},{5,6}}", _blk {
    //     it("includes 1", _blk {
    //         std::map<int,int> m = {{1,2},{3,4},{5,6}};
    //         expect(m).to_include({1})();
    //       });
    //     it("includes [1,2,3]", _blk {
    //         std::map<int,int> m = {{1,2},{3,4},{5,6}};
    //         expect(m).to_include({1,2,3})();
    //       });
    //     it("does not include 4", _blk {
    //         std::map<int,int> m = {{1,2},{3,4},{5,6}};
    //         expect(m).not_().to_include({4})();
    //       });
    //     it("does not include 4,5,6", _blk {
    //         std::map<int,int> m = {{1,2},{3,4},{5,6}};
    //         expect({1,2,3}).not_().to_include({4,5,6});
    //       });
    //   });

    // before("each", _blk {
    //     n = rand();
    //   });

    // context("argument is positive", _blk {
    //     it("return positive", _blk {
    //         expect_to_equal(fabs(n), n);
    //       });
    //   });

    // context("argument is negative", _blk {
    //     it("return positive", _blk {
    //         expect(fabs(-n).to(equal(n));
    //       });
    //   });
  });

/* Here is the description of strcmp */
// describe strcmp_spec("int strcmp ( const char * str1, const char * str2 )", _blk {
//   it( "returns 0 only when strings are equal", _blk {
//       // should_equal( strcmp("hello", "hello"), 0);
//       // should_not_be_null( strcmp("hello", "world") );
//   });

//   it( "returns a negative integer when str1 is less than str2", _blk {
//       // should_be_true( strcmp("hello", "world") < 0 );
//       // should_be_true( strcmp("0123", "1321431") < 0 );
//   });

//   it( "returns a positive integer if str1 is greater than str2", _blk {
//       // should_be_true( strcmp("yellow", "world") > 0 );
//       // should_be_true( strcmp("9", "789") > 0 );
//   });
// });

describe_some<std::vector<int>> vector_spec({1,2,3,4}, _blk {
    it("should contain 2", _blk {
        expect({1,2,3,4}).to_include(2)();
      });
  });

/* Here is the declaration of fabs description defined in an other file (fabs_spec.c in this sample)*/
int main(){
  bool r = true;
  r &= bool_spec.run();
  r &= vector_spec.run();
// #undef expect
// #undef it
//  Description d("halp", _blk {});
//  It i =  d.it("me", _blk {});
//  auto expectation =  i.expect(2);
//  auto be_between = expectation.to_be_between(1,3);
//  be_between();
  //strcmp_spec.run();
  return r ? EXIT_SUCCESS : EXIT_FAILURE;
}


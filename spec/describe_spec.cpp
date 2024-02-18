#include "cppspec.hpp"

using namespace CppSpec;

describe describe_spec("Description", $ {
  it("responds to it", _ { });
  context<int>("can create ClassContexts", 1, _{});
});


CPPSPEC_MAIN(describe_spec);
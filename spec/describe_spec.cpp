#include "cppspec.hpp"

using namespace CppSpec;

describe describe_spec("Description", $ {
  it("responds to it", _ { });
  context<int>("can create ClassContexts", 1, _{});
});


Runner::spec_group describe_spec_group([](Runner &r){
  r.add_spec(describe_spec);
});
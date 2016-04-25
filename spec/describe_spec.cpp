#include "cppspec.hpp"

using namespace CppSpec;

describe describe_spec("Description has", $ {
  it("responds to it", _ {
    
  });
});


Runner::spec_group describe_spec_group([](Runner &r){
  r.add_spec(describe_spec);
});
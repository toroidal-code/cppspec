#include "cppspec.hpp"

using namespace CppSpec;

describe base_describe_spec("Base description", $ { });

describe describe_statements_spec("Description has", $ {
  it("responds to it", _ {
    
  });
});
  

int main() {
  bool r = true;
  r &= base_describe_spec.run(Printer::terse);
  r &= describe_statements_spec.run(Printer::terse);
  return r ? EXIT_SUCCESS : EXIT_FAILURE;
}

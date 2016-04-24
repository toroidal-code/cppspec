#include "cppspec.hpp"

using namespace CppSpec;

describe base_describe_spec("Base description", $ { });

describe describe_statements_spec("Description has", $ {
  it("responds to it", _ {
    
  });
});
  

int main() {
  return Runner()
      .add_spec(base_describe_spec)
      .add_spec(describe_statements_spec)
      .exec() ? EXIT_SUCCESS : EXIT_FAILURE;
}

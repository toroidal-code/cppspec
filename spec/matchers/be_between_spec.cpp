#include "cppspec.hpp"
using namespace CppSpec;
describe be_between_spec("BeBetween", $ {
//  class SizeMatters : public Matchers::BaseMatcher<std::string, std::string> {
//    SizeMatters(Expectation<std::string> &expectation, std::string expected)
//        : Matchers::BaseMatcher<std::string,std::string>(expectation, expected){};
//    bool match() { return this->get_actual() == this->get_expected(); }
//  };

  context("expect(...).to_be_between(min, max) (inclusive)", _ {
//    it_behaves_like("be_between", &Matchers::BeBetween::inclusive, _ {
//      Matchers::BeBetween matcher(auto min, auto max) {
//        return Matchers::BeBetween(self, min, max);
//      }
//    });

    it("is inclusive", _ {
      expect(1).to_be_between(1, 10);
      expect(10).to_be_between(1, 10);
    });
  });

  context("expect(...).to_be_between(min, max) (exclusive)", _ {
//    it_behaves_like("be_between", &Matchers::BeBetween::inclusive, _ {
//      Matchers::BeBetween matcher(auto min, auto max) {
//        return Matchers::BeBetween(self, min, max);
//      }
//    });

    it("is not inclusive", _ {
      expect(1).not_().to_be_between(1, 10, Matchers::RangeMode::exclusive);
      expect(10).not_().to_be_between(1, 10, Matchers::RangeMode::exclusive);
    });
  });

});

//int main() { return EXIT_SUCCESS; }
//extern Runner runner;
//auto nil = runner.add_spec(expectation_spec);

Runner::spec_group be_between_spec_group([](Runner &r){
  r.add_spec(be_between_spec);
});
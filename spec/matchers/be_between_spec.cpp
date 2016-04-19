#include "cppspec.hpp"

//describe be_between_spec("BeBetween", $ {
//  class SizeMatters : public Matchers::BaseMatcher<std::string, std::string> {
//    SizeMatters(Expectations::Expectation<std::string> &expectation, std::string expected)
//        : Matchers::BaseMatcher<std::string,std::string>(expectation, expected){};
//    bool match() { return this->get_actual() == this->get_expected(); }
//  };
//
//  describe("expect(...).to_be_between(min, max) (inclusive)", _ {
//    it_behaves_like("be_between", &Matchers::BeBetween::inclusive, _ {
//      Matchers::BeBetween matcher(auto min, auto max) {
//        return Matchers::BeBetween(self, min, max);
//      }
//    });
//
//    it("is inclusive", _ {
//      expect(1).to_be_between(1, 10).run();
//      expect(10).to_be_between(1, 10).run();
//    });
//
//    it("indicates it was not comparable if it does not respond to <= and >=", _ {
//      expect([=]{
//        expect(nullptr).to_be_between(0,10).run();
//      }).to_fail_with("expected nullptr to be between 0 and 10")
//    });
//  });
//});

int main() { return EXIT_SUCCESS; }
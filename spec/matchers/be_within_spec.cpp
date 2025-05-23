#include <source_location>

#include "cppspec.hpp"
#include "formatters/formatters_base.hpp"

using namespace CppSpec;

/**
 * @file
 *
 * Adapted from rpsec-expectation's be_within_spec.rb
 * which is copyrighted under the MIT License
 * to the RSpec team from 2005-2016
 */

// clang-format off
describe be_within_spec("expect(actual).to_be_within(delta).of(expected)", $  {
  it("passes when actual == expected", _ {
    expect(5.0).to_be_within(0.5).of(5.0);
  });

  it("passes when actual < (expected + delta)", _ {
    expect(5.49).to_be_within(0.5).of(5.0);
  });

  it("passes when actual > (expected - delta)", _ {
    expect(4.51).to_be_within(0.5).of(5.0);
  });

  it("passes when actual == (expected + delta)", _ {
    expect(5.5).to_be_within(0.5).of(5.0);
  });

  it("passes when actual == (expected - delta)", _ {
    expect(4.5).to_be_within(0.5).of(5.0);
  });

  it("passes with integer arguments that are near each other", _ {
    expect(1.0001).to_be_within(5).percent_of(1);
  });

  it("passes with negative arguments", _ {
    expect(-1.0001).to_be_within(5).percent_of(-1);
  });

  it("works with std::time", _ {
    expect(std::time(nullptr)).to_be_within(0.1).of(std::time(nullptr));
  });

//  it("fails when actual < (expected - delta)", _ {
//    expect([]{
//      expect(4.49).to_be_within(0.5).of(5.0);
//    }).to_fail_with("expected 4.49 to be within 0.5 of 5.0");
//  });
//
//  it("fails when actual > (expected + delta)", _ {
//    expect([]{
//      expect(5.51).to_be_within(0.5).of(5.0);
//    }).to_fail_with("expected 5.51 to be within 0.5 of 5.0");
//  });

  it("provides a description", _ {
    double d = 5.1;
    ExpectationValue ex(self, d, std::source_location::current());
    Matchers::BeWithin matcher = Matchers::BeWithinHelper(ex, 0.5).of(5.0);
    expect(matcher.description()).to_equal("be within 0.5 of 5");
  });

  context("expect(actual).to_be_within(delta).percent_of(expected)", _ {
    it("passes when actual is within the given percent variance", _ {
      expect(9).to_be_within(10).percent_of(10);
      expect(10).to_be_within(10).percent_of(10);
      expect(11).to_be_within(10).percent_of(10);
    });

    it("fails when actual is outside the given percent variance", _ {
      auto base_formatter = Formatters::BaseFormatter();
      auto ex = ExpectationValue(20.1, std::source_location::current());
      auto matcher = Matchers::BeWithinHelper(ex, 10).percent_of(10.0);
      expect(matcher.run()).to_fail_with("expected 20.1 to be within 10% of 10");
    });

    it("provides a description", _ {
      auto d = 5.1;
      ExpectationValue<double> ex(self, d, std::source_location::current());
      Matchers::BeWithin matcher = Matchers::BeWithinHelper(ex, 0.5).percent_of(5.0);
      expect(matcher.description()).to_equal("be within 0.5% of 5");
    });
  });

  context("expect(actual).not_().to_be_within(delta).of(expected)", _ {
    it("passes when actual < (expected - delta)", _ {
      expect(4.49).not_().to_be_within(0.5).of(5.0);
    });

    it("passes when actual > (expected + delta)", _ {
      expect(5.51).not_().to_be_within(0.5).of(5.0);
    });

    // it("fails when actual == expected", _ {
    //   expect([=]{
    // 	expect(5.0).not_().to_be_within(0.5).of(5.0);
    //   }).to_fail_with("expected 5.0 not to be within 0.5 of 5.0");
    // });

    // it("fails when actual < (expected + delta)", _ {
    //   expect([=]{
    // 	expect(5.49).not_().to_be_within(0.5).of(5.0);
    //   }).to_fail_with("expected 5.49 not to be within 0.5 of 5.0");
    // });

    // it("fails when actual > (expected - delta)", _ {
    //   expect([=]{
    // 	expect(4.51).not_().to_be_within(0.5).of(5.0);
    //   }).to_fail_with("expected 4.51 not to be within 0.5 of 5.0");
    // });

    // it("fails when actual == (expected - delta)", _ {
    //   expect([=]{
    // 	expect(4.5).not_().to_be_within(0.5).of(5.0);
    //   }).to_fail_with("expected 4.5 not to be within 0.5 of 5.0");
    // });

    // it("fails when actual == (expected + delta)", _ {
    //   expect([=]{
    // 	expect(5.5).not_().to_be_within(0.5).of(5.0);
    //   }).to_fail_with("expected 5.5 not to be within 0.5 of 5.0");
    // });

    // it("passes if the actual is not numeric", _ {
    //   expect(nullptr).not_().to_be_within(0.1).of(0);
    // });
  });
});


CPPSPEC_MAIN(be_within_spec);

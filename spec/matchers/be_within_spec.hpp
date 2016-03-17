#include "cppspec.hpp"

describe be_within_spec(
  "expect(actual).to_be_within(delta).of(expected)", _  {
	it("passes when actual == expected", _ {
		expect(5.0).to_be_within(0.5).of(5.0)();
	});

	it("passes when actual < (expected + delta)", _ {
		expect(5.49).to_be_within(0.5).of(5.0)();
	});

	it("passes when actual > (expected - delta)", _ {
		expect(4.51).to_be_within(0.5).of(5.0)();
	});

	it("passes when actual == (expected + delta)", _ {
		expect(5.5).to_be_within(0.5).of(5.0)();
	});	

	it("passes when actual == (expected - delta)", _ {
		expect(4.5).to_be_within(0.5).of(5.0)();
	});	

	it("passes with integer arguments that are near each other", _ {
		expect(1.0001).to_be_within(5).percent_of(1)();
	});	

	it("passes with negative arguments", _ {
		expect(-1.0001).to_be_within(5).percent_of(-1)();
	});

	// Do failures here

	it("provides a description", _ {
		auto ex(5.1);
		Matchers::BeWithin<float, float> matcher(ex, 0.5);
		matcher = matcher.of(5.0);
		matcher.matches(5.1);
		expect(matcher.description()).to_equal(std::string("be within 0.5 of 5"))();
	});

	context("expect(actual).to_be_within(delta).percent_of(expected)", _ {
		it("passes when actual is within the given percent variance", _ {
			expect(9).to_be_within(10).percent_of(10)();
			expect(10).to_be_within(10).percent_of(10)();
			expect(11).to_be_within(10).percent_of(10)();
		});

		// TODO: PEDNING
		// it("fails when actual is outside the given percent variance", _ {
		// 	expect([]{
		// 		expect(8.9).to_be_within(10).percent_of(10.0)();
		// 	}).to_fail_with("expected 8.9 to be within 10% of 10.0")();

		// 	expect([]{
		// 		expect(11.1).to_be_within(10).percent_of(10.0)();
		// 	}).to_fail_with("expected 11.1 to be within 10% of 10.0")();
		// });

		it("provides a description", _ {
			auto ex(5.1);
			Matchers::BeWithin<float, float> matcher(ex, 0.5);
			matcher = matcher.percent_of(5.0);
			matcher.matches(5.1);
			expect(matcher.description()).to_equal(std::string("be within 0.5% of 5"))();
		});
	});

	context("expect(actual).not_().to_be_within(delta).of(expected)", _ {
		it("passes when actual < (expected - delta)", _ {
			expect(4.49).not_().to_be_within(0.5).of(5.0)();
		});

		it("passes when actual > (expected + delta)", _ {
			expect(5.51).not_().to_be_within(0.5).of(5.0)();
		});

		// it("fails when actual == expected", _ {
		// 	expect([]{
		// 		expect(5.51).not_().to_be_within(0.5).of(5.0)();
		// 	}).to_fail_with("expected 5.0 not to be within 0.5 of 5.0")();
		// });

	});


});

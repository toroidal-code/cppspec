#include <functional>
#include <stdexcept>

#include "cppspec.hpp"

using namespace CppSpec;

struct MyException : public std::exception {
  [[nodiscard]] const char* what() const noexcept override { return "MyException"; }
};

struct OtherException : public std::exception {
  [[nodiscard]] const char* what() const noexcept override { return "OtherException"; }
};

describe throw_spec("to_throw matcher", $ {
  context("basic throw detection", _ {
    it("passes when function throws std::exception", _ {
      std::function<void*()> f = [] -> void* { throw std::runtime_error("boom"); };
      expect(f).to_throw();
    });

    it("fails when function does not throw", _ {
      std::function<int()> f = [] { return 42; };
      expect(f).not_().to_throw();
    });

    it("passes when lambda throws int", _ {
      std::function<void*()> f = [] -> void* { throw 42; };
      expect(f).template to_throw<int>();
    });
  });

  context("typed exception matching", _ {
    it("catches specific exception type", _ {
      std::function<void*()> f = [] -> void* { throw MyException{}; };
      expect(f).template to_throw<MyException>();
    });

    it("catches base class when derived thrown", _ {
      std::function<void*()> f = [] -> void* { throw MyException{}; };
      expect(f).template to_throw<std::exception>();
    });

    it("does not catch wrong exception type", _ {
      std::function<void*()> f = [] -> void* { throw OtherException{}; };
      expect(f).not_().template to_throw<MyException>();
    });

    it("catches std::runtime_error specifically", _ {
      std::function<void*()> f = [] -> void* { throw std::runtime_error("err"); };
      expect(f).template to_throw<std::runtime_error>();
    });

    it("catches std::logic_error specifically", _ {
      std::function<void*()> f = [] -> void* { throw std::logic_error("logic"); };
      expect(f).template to_throw<std::logic_error>();
    });
  });

  context("non-throwing functions", _ {
    it("passes not_().to_throw() for a returning function", _ {
      std::function<int()> f = [] { return 1 + 1; };
      expect(f).not_().to_throw();
    });

    it("does not throw for a constant function", _ {
      std::function<int()> f = [] { return 0; };
      expect(f).not_().to_throw();
    });
  });

  context("functions with side effects before throwing", _ {
    it("still detects throw after side effects", _ {
      int x = 0;
      std::function<void*()> f = [&x] -> void* {
        x = 1;
        throw std::runtime_error("after side effect");
      };
      expect(f).to_throw();
    });
  });
});

CPPSPEC_MAIN(throw_spec);

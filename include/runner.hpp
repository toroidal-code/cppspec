#ifndef CPPSPEC_RUNNER_HPP
#define CPPSPEC_RUNNER_HPP
#include <list>
#include "printer.hpp"

namespace CppSpec {

class Runner {
  std::list<Description *> specs;
  PrettyPrinter &printer = Printer::terse;

 public:
  Runner() = default;

  Runner(PrettyPrinter &printer) : printer(printer){};

  template <typename T>
  Runner &add_spec(ClassDescription<T> &spec);
  Runner &add_spec(Description &spec);
  Result run(BasePrinter &printer);
  Result exec();
};

template <typename T>
Runner &Runner::add_spec(ClassDescription<T> &spec) {
  specs.push_back(&spec);
  return *this;
}

Runner &Runner::add_spec(Description &spec) {
  specs.push_back(&spec);
  return *this;
}

Result Runner::run(BasePrinter &printer) {
  bool success = true;
  for (auto spec : specs) {
    success &= static_cast<bool>(spec->run(printer));
  }
  return success ? Result::success : Result::failure;
}

Result Runner::exec() {
  if (this->printer.mode == BasePrinter::Mode::terse) {
    auto failures = std::make_shared<std::list<std::string>>();
    PrettyPrinter printer(this->printer.mode, failures);
    Result result = this->run(printer);
    std::cout << RED << std::endl << Util::join<std::list<std::string>>(*failures, "\n\n") << RESET << std::endl;
    return result;
  } else {
    return this->run(printer);
  }
}
}
#endif  // CPPSPEC_RUNNER_HPP

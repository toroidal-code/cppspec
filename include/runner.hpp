/** @file */
#ifndef CPPSPEC_RUNNER_HPP
#define CPPSPEC_RUNNER_HPP
#include "formatters/verbose.hpp"
#include "formatters/progress.hpp"
#include "formatters/tap.hpp"

namespace CppSpec {

class Runner {
  std::list<Description *> specs;
  Formatters::BaseFormatter &formatter;

 public:
  typedef std::function<void(Runner&)> spec_group;
  Runner(Formatters::BaseFormatter &formatter = Formatters::progress) : formatter(formatter){};

  template <typename T>
  Runner &add_spec(ClassDescription<T> &spec);
  Runner &add_spec(Description &spec);
  Runner &add_spec_group(spec_group);
  Result run(Formatters::BaseFormatter &printer);
  Result exec();
};

template <typename T>
Runner &Runner::add_spec(ClassDescription<T> &spec) {
  specs.push_back(&spec);
  return *this;
}

inline Runner &Runner::add_spec(Description &spec) {
  specs.push_back(&spec);
  return *this;
}

inline Runner &Runner::add_spec_group(spec_group r) {
  r(*this);
  return *this;
}

inline Result Runner::run(Formatters::BaseFormatter &formatter) {
  bool success = true;
  for (auto spec : specs) {
    success &= static_cast<bool>(spec->run(formatter));
  }
  return success ? Result::success() : Result::failure();
}

inline Result Runner::exec() {
  formatter.set_multiple(true);
  Result result = this->run(formatter);
  formatter.cleanup();
  return result;
}

}
#endif  // CPPSPEC_RUNNER_HPP

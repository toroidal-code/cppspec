/** @file */
#ifndef CPPSPEC_RUNNER_HPP
#define CPPSPEC_RUNNER_HPP
#include "formatters/formatters_base.hpp"
#pragma once

#include <list>

#include "argparse.hpp"

namespace CppSpec {

/**
 * @brief A collection of Descriptions that are run in sequence
 */
class Runner {
  std::list<Description *> specs;
  Formatters::BaseFormatter *formatter;

 public:
  using spec_group = std::function<void(Runner &)>;
  explicit Runner() = default;

  ~Runner() { delete formatter; }

  /**
   * @brief Add a describe_a suite to the list of suites to run
   *
   * @param spec the spec to be added
   * @return a reference to the modified Runner
   */
  template <typename T>
  Runner &add_spec(ClassDescription<T> &spec) {
    specs.push_back(&spec);
    return *this;
  }

  /**
   * @brief Add a Description object
   *
   * @param spec the spec to be added
   * @return a reference to the modified Runner
   */
  Runner &add_spec(Description &spec) {
    specs.push_back(&spec);
    return *this;
  }

  Result run(Formatters::BaseFormatter &formatter) {
    bool success = true;
    for (auto *spec : specs) {
      success &= static_cast<bool>(spec->run(formatter));
    }
    return success ? Result::success() : Result::failure();
  }

  template <typename Formatter>
  Result exec() {
    Formatter formatter;
    if (specs.size() > 1) {
      formatter.set_multiple(true);
    }
    Result result = run(formatter);
    return result;
  }
};

}  // namespace CppSpec
#endif  // CPPSPEC_RUNNER_HPP

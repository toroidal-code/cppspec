/** @file */
#ifndef CPPSPEC_RUNNER_HPP
#define CPPSPEC_RUNNER_HPP
#include <initializer_list>
#pragma once

#include <list>
#include <utility>

#include "description.hpp"
#include "formatters/formatters_base.hpp"
#include "result.hpp"

namespace CppSpec {

/**
 * @brief A collection of Descriptions that are run in sequence
 */
class Runner {
  std::list<Description *> specs{};
  std::shared_ptr<Formatters::BaseFormatter> formatter;

 public:
  explicit Runner(std::shared_ptr<Formatters::BaseFormatter> formatter)
      : formatter{std::move(formatter)} {};

  ~Runner() = default;

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

  Result run() {
    bool success = true;
    for (auto *spec : specs) {
      success &= static_cast<bool>(spec->run(*formatter));
    }
    return success ? Result::success() : Result::failure();
  }

  Result exec() {
    if (specs.size() > 1) {
      formatter->set_multiple(true);
    }
    Result result = run();
    return result;
  }
};

}  // namespace CppSpec
#endif  // CPPSPEC_RUNNER_HPP

/** @file */
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
  std::list<Description*> specs;
  std::list<std::shared_ptr<Formatters::BaseFormatter>> formatters;

 public:
  template <typename... Formatters>
  explicit Runner(Formatters&&... formatters) : formatters{std::forward<Formatters>(formatters)...} {}

  explicit Runner(std::list<std::shared_ptr<Formatters::BaseFormatter>>&& formatters)
      : formatters{std::move(formatters)} {}

  /**
   * @brief Add a Description object
   *
   * @param spec the spec to be added
   * @return a reference to the modified Runner
   */
  Runner& add_spec(Description& spec) {
    specs.push_back(&spec);
    return *this;
  }

  template <typename... Specs>
  Runner& add_specs(Specs&... specs) {
    (add_spec(specs), ...);  // Fold expression to add all specs
    return *this;
  }

  Result run(std::source_location location = std::source_location::current()) {
    bool success = true;
    for (Description* spec : specs) {
      spec->timed_run();
      success &= spec->get_result().status();
    }
    for (auto& formatter : formatters) {
      for (Description* spec : specs) {
        formatter->format(static_cast<Runnable&>(*spec));
      }
    }
    return success ? Result::success(location) : Result::failure(location);
  }

  Result exec() { return run(); }
};

}  // namespace CppSpec

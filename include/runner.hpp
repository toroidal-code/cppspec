/** @file */
#pragma once
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
  std::list<Description*> specs{};
  std::shared_ptr<Formatters::BaseFormatter> formatter;

 public:
  explicit Runner(std::shared_ptr<Formatters::BaseFormatter> formatter) : formatter{std::move(formatter)} {};

  ~Runner() = default;

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
      formatter->format(static_cast<Runnable&>(*spec));
      success &= spec->get_result().status();
    }
    return success ? Result::success(location) : Result::failure(location);
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

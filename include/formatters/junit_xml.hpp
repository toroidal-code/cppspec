#pragma once
#include <algorithm>
#include <atomic>
#include <chrono>
#include <filesystem>
#include <numeric>
#include <string>

#include "formatters_base.hpp"
#include "it_base.hpp"

namespace CppSpec::Formatters {
// JUnit XML header
constexpr static auto junit_xml_header = R"(<?xml version="1.0" encoding="UTF-8"?>)";

struct XMLSerializable {
  virtual ~XMLSerializable() = default;
  [[nodiscard]] virtual std::string to_xml() const = 0;
};

namespace JUnitNodes {
struct Result {
  enum class Status { Failure, Error, Skipped };
  Status status = Status::Failure;
  std::string message;
  std::string type;
  std::string text;

  Result(std::string message, std::string type, std::string text, Status status = Status::Failure)
      : status(status), message(std::move(message)), type(std::move(type)), text(std::move(text)) {}

  [[nodiscard]] std::string status_string() const {
    switch (status) {
      case Status::Failure:
        return "failure";
      case Status::Error:
        return "error";
      case Status::Skipped:
        return "skipped";
    }
    return "failure";  // Default to failure if status is unknown
  }

  [[nodiscard]] std::string to_xml() const {
    return std::format(R"(      <{} message="{}" type="{}">{}</{}>)", status_string(), message, type, text,
                       status_string());
  }
};

struct TestCase {
  std::string name;
  std::string classname;
  std::size_t assertions = 0;
  std::chrono::duration<double> time;
  std::list<Result> results;
  std::string file;
  std::size_t line;

  [[nodiscard]] std::string to_xml() const {
    auto start =
        std::format(R"(    <testcase name="{}" classname="{}" assertions="{}" time="{:f}", file="{}" line="{}")", name,
                    classname, assertions, time.count(), file, line);
    if (results.empty()) {
      return start + "/>";
    }

    auto xml_results = results | std::views::transform([](const Result& r) { return r.to_xml(); });

    std::stringstream ss;
    ss << start << ">" << std::endl;

    ss << *std::ranges::fold_left_first(xml_results,
                                        [](const std::string& acc, const std::string& r) { return acc + "\n" + r; });
    ss << std::endl;
    ss << "    </testcase>";
    return ss.str();
  }
};

struct TestSuite {
  size_t id;
  std::string name;
  std::chrono::duration<double> time;
  std::chrono::time_point<std::chrono::system_clock> timestamp;
  std::size_t tests;
  std::size_t failures;
  std::list<TestCase> cases;

  TestSuite(std::string name,
            std::chrono::duration<double> time,
            size_t tests,
            size_t failures,
            std::chrono::time_point<std::chrono::system_clock> timestamp)
      : id(get_next_id()), name(std::move(name)), time(time), timestamp(timestamp), tests(tests), failures(failures) {}

  [[nodiscard]] std::string to_xml() const {
    auto timestamp_str =
        std::format("{0:%F}T{0:%T}", std::chrono::zoned_time(std::chrono::current_zone(), timestamp).get_local_time());

    std::stringstream ss;
    ss << "  "
       << std::format(R"(<testsuite id="{}" name="{}" time="{:f}" timestamp="{}" tests="{}" failures="{}">)", id, name,
                      time.count(), timestamp_str, tests, failures);
    ss << std::endl;
    for (const auto& test_case : cases) {
      ss << test_case.to_xml() << std::endl;
    }
    ss << "  </testsuite>";
    return ss.str();
  }

  static size_t get_next_id() {
    static std::atomic_size_t id_counter = 0;
    return id_counter++;
  }
};

struct TestSuites {
  std::string name;
  size_t tests = 0;
  size_t failures = 0;
  std::chrono::duration<double> time{};
  std::chrono::time_point<std::chrono::system_clock> timestamp;

  std::list<TestSuite> suites;

  [[nodiscard]] std::string to_xml() const {
    std::stringstream ss;
    auto timestamp_str = std::format("{0:%F}T{0:%T}", timestamp);
    ss << std::format(R"(<testsuites name="{}" tests="{}" failures="{}" time="{:f}" timestamp="{}">)", name, tests,
                      failures, time.count(), timestamp_str);
    ss << std::endl;
    for (const auto& suite : suites) {
      ss << suite.to_xml() << std::endl;
    }
    ss << "</testsuites>" << std::endl;
    return ss.str();
  }
};
}  // namespace JUnitNodes

class JUnitXML : public BaseFormatter {
  JUnitNodes::TestSuites test_suites;

 public:
  explicit JUnitXML(std::ostream& out_stream = std::cout, bool color = is_terminal())
      : BaseFormatter(out_stream, color) {}

  ~JUnitXML() {
    test_suites.tests =
        std::accumulate(test_suites.suites.begin(), test_suites.suites.end(), 0,
                        [](size_t sum, const JUnitNodes::TestSuite& suite) { return sum + suite.tests; });
    test_suites.failures =
        std::accumulate(test_suites.suites.begin(), test_suites.suites.end(), 0,
                        [](size_t sum, const JUnitNodes::TestSuite& suite) { return sum + suite.failures; });
    test_suites.time = std::ranges::fold_left(test_suites.suites, std::chrono::duration<double>(0),
                                              [](const auto& acc, const auto& suite) { return acc + suite.time; });
    test_suites.timestamp = test_suites.suites.front().timestamp;

    out_stream << std::fixed;  // disable scientific notation
    // out_stream << std::setprecision(6); // set precision to 6 decimal places
    out_stream << junit_xml_header << std::endl;
    out_stream << test_suites.to_xml() << std::endl;
    out_stream.flush();
  }

  void format(Description& description) override {
    if (test_suites.name.empty()) {
      std::filesystem::path file_path = description.get_location().file_name();
      test_suites.name = file_path.stem().string();
    }
    if (description.has_parent()) {
      return;
    }
    test_suites.suites.emplace_back(description.get_description(), description.get_runtime(), description.num_tests(),
                                    description.num_failures(), description.get_start_time());
  }

  void format(ItBase& it) override {
    using namespace std::chrono;
    std::forward_list<std::string> descriptions;

    descriptions.push_front(it.get_description());
    for (auto* parent = it.get_parent_as<Description>(); parent->has_parent();
         parent = parent->get_parent_as<Description>()) {
      descriptions.push_front(parent->get_description());
    }

    std::string description = Util::join(descriptions, " ");

    auto test_case = JUnitNodes::TestCase{
        .name = description,
        .classname = "",
        .assertions = it.get_results().size(),
        .time = it.get_runtime(),
        .results = {},
        .file = it.get_location().file_name(),
        .line = it.get_location().line(),
    };

    for (auto& result : it.get_results()) {
      if (result.is_success()) {
        continue;
      }
      test_case.results.emplace_back(result.get_location_string() + ": Match failure.", result.get_type(),
                                     result.get_message());
    }

    test_suites.suites.back().cases.push_back(test_case);
  }
};
}  // namespace CppSpec::Formatters

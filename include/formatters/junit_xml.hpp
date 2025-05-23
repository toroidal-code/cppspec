#pragma once
#include <algorithm>
#include <chrono>
#include <numeric>
#include <string>

#ifndef CPPSPEC_SEMIHOSTED
#include <filesystem>
#endif

#include "formatters_base.hpp"
#include "it_base.hpp"

namespace CppSpec::Formatters {
// JUnit XML header
constexpr static auto junit_xml_header = R"(<?xml version="1.0" encoding="UTF-8"?>)";

inline std::string encode_xml(const std::string& data) {
  std::string buffer;
  for (char c : data) {
    switch (c) {
      case '<':
        buffer += "&lt;";
        break;
      case '>':
        buffer += "&gt;";
        break;
      case '&':
        buffer += "&amp;";
        break;
      case '"':
        buffer += "&quot;";
        break;
      case '\'':
        buffer += "&apos;";
        break;
      default:
        buffer += c;
    }
  }
  return buffer;
}

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
    return std::format(R"(      <{} message="{}" type="{}">{}</{}>)", status_string(), encode_xml(message),
                       encode_xml(type), encode_xml(text), status_string());
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
        std::format(R"(    <testcase name="{}" classname="{}" assertions="{}" time="{:f}" file="{}" line="{}")",
                    encode_xml(name), encode_xml(classname), assertions, time.count(), file, line);
    if (results.empty()) {
      return start + "/>";
    }

    auto xml_results = results | std::views::transform([](const Result& r) { return r.to_xml(); });

    std::stringstream ss;
    ss << start << ">" << std::endl;

    ss << std::accumulate(xml_results.begin(), xml_results.end(), std::string{},
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
    std::string timestamp_str;
#if defined(__APPLE__) || defined(CPPSPEC_SEMIHOSTED)
    // Cludge because macOS doesn't have std::chrono::current_zone() or std::chrono::zoned_time()
    std::time_t time_t_timestamp = std::chrono::system_clock::to_time_t(timestamp);
    std::tm localtime = *std::localtime(&time_t_timestamp);
    std::ostringstream oss;
    oss << std::put_time(&localtime, "%Y-%m-%dT%H:%M:%S");
    timestamp_str = oss.str();
#else
    // Use std::chrono::current_zone() and std::chrono::zoned_time() if available (C++20)
    auto localtime = std::chrono::zoned_time(std::chrono::current_zone(), timestamp).get_local_time();
    timestamp_str = std::format("{0:%F}T{0:%T}", localtime);
#endif

    std::stringstream ss;
    ss << "  "
       << std::format(R"(<testsuite id="{}" name="{}" time="{:f}" timestamp="{}" tests="{}" failures="{}">)", id,
                      encode_xml(name), time.count(), timestamp_str, tests, failures);
    ss << std::endl;
    for (const TestCase& test_case : cases) {
      ss << test_case.to_xml() << std::endl;
    }
    ss << "  </testsuite>";
    return ss.str();
  }

  static size_t get_next_id() {
    static std::size_t id_counter = 0;
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
    ss << std::format(R"(<testsuites name="{}" tests="{}" failures="{}" time="{:f}" timestamp="{}">)", encode_xml(name),
                      tests, failures, time.count(), timestamp_str);
    ss << std::endl;
    for (const TestSuite& suite : suites) {
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
        std::accumulate(test_suites.suites.begin(), test_suites.suites.end(), size_t{0},
                        [](size_t sum, const JUnitNodes::TestSuite& suite) { return sum + suite.tests; });
    test_suites.failures =
        std::accumulate(test_suites.suites.begin(), test_suites.suites.end(), size_t{0},
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

  void format(const Description& description) override {
    if (test_suites.name.empty()) {
#ifdef CPPSPEC_SEMIHOSTED
      std::string file_path = description.get_location().file_name();
      // remove leading folders
      auto pos = file_path.find_last_of("/");
      if (pos != std::string::npos) {
        file_path = file_path.substr(pos + 1);
      }

      // remove extension
      pos = file_path.find_last_of('.');
      if (pos != std::string::npos) {
        file_path = file_path.substr(0, pos);
      }

      test_suites.name = file_path;
#else
      std::filesystem::path file_path = description.get_location().file_name();
      test_suites.name = file_path.stem().string();
#endif
    }
    if (description.has_parent()) {
      return;
    }
    test_suites.suites.emplace_back(description.get_description(), description.get_runtime(), description.num_tests(),
                                    description.num_failures(), description.get_start_time());
  }

  void format(const ItBase& it) override {
    using namespace std::chrono;
    std::forward_list<std::string> descriptions;

    descriptions.push_front(it.get_description());
    for (const auto* parent = it.get_parent_as<Description>(); parent->has_parent();
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

    for (const Result& result : it.get_results()) {
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

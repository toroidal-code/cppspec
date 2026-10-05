#include <chrono>
#include <regex>
#include <sstream>
#include <string>

#include "cppspec.hpp"
#include "formatters/junit_xml.hpp"

using namespace CppSpec;

// clang-format off
describe junit_xml_spec("Formatters::JUnitXML", $ {
  context("with no suites", _ {
    it("writes an empty <testsuites> element", _ {
      std::ostringstream out;
      { Formatters::JUnitXML formatter(out, false); }
      std::string xml = out.str();

      expect(xml).to_start_with(R"(<?xml version="1.0" encoding="UTF-8"?>)");
      expect(xml.contains(R"(tests="0" failures="0")")).to_be_true();
      expect(xml.contains("</testsuites>")).to_be_true();
      expect(xml.contains("<testsuite ")).to_be_false();
    });
  });

  context("timestamps", _ {
    it("formats <testsuites> and <testsuite> in the same time zone", _ {
      auto now = std::chrono::system_clock::now();
      Formatters::JUnitNodes::TestSuites suites{.name = "suites", .timestamp = now};
      suites.suites.emplace_back("suite", std::chrono::duration<double>(0), 0, 0, now);
      std::string xml = suites.to_xml();

      std::regex timestamp_attr(R"re(timestamp="([^"]*)")re");
      std::vector<std::string> timestamps;
      for (auto m = std::sregex_iterator(xml.begin(), xml.end(), timestamp_attr); m != std::sregex_iterator(); ++m) {
        timestamps.push_back((*m)[1].str());
      }

      expect(timestamps.size()).to_equal(2UL);
      expect(timestamps[0]).to_equal(timestamps[1]);
    });
  });
});

CPPSPEC_MAIN(junit_xml_spec);

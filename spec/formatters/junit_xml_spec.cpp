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
});

CPPSPEC_MAIN(junit_xml_spec);

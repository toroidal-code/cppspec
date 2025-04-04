#pragma once

#include <argparse/argparse.hpp>
#include <fstream>
#include <string>
#include <string_view>

#include "formatters/junit_xml.hpp"
#include "formatters/progress.hpp"
#include "formatters/tap.hpp"
#include "formatters/verbose.hpp"
#include "runner.hpp"

namespace CppSpec {

inline std::string file_name(std::string_view path) {
  std::string_view file = path;
  for (size_t i = 0; i < path.size(); ++i) {
    if (path[i] == '/') {
      file = &path[i + 1];
    }
  }
  return std::string{file};
}

inline Runner parse(int argc, char** const argv) {
  argparse::ArgumentParser program{file_name(argv[0])};

  program.add_argument("-f", "--format")
      .default_value(std::string{"p"})
      .choices("progress", "p", "tap", "t", "detail", "d", "junit", "j")
      .required()
      .help("set the output format");

  program.add_argument("--output-junit").help("output JUnit XML to the specified file").default_value(std::string{});
  program.add_argument("--verbose").help("increase output verbosity").flag();

  try {
    program.parse_args(argc, argv);
  } catch (const std::runtime_error& err) {
    std::cerr << err.what() << std::endl;
    std::cerr << program;
    std::exit(1);
  }

  auto format_string = program.get<std::string>("--format");
  std::shared_ptr<Formatters::BaseFormatter> formatter;
  if (format_string == "d" || format_string == "detail" || program["--verbose"] == true) {
    formatter = std::make_shared<Formatters::Verbose>();
  } else if (format_string == "p" || format_string == "progress") {
    formatter = std::make_shared<Formatters::Progress>();
  } else if (format_string == "t" || format_string == "tap") {
    formatter = std::make_shared<Formatters::TAP>();
  } else if (format_string == "j" || format_string == "junit") {
    formatter = std::make_shared<Formatters::JUnitXML>();
  } else {
    std::cerr << "Unrecognized format type" << std::endl;
    std::exit(-1);
  }

  auto junit_output_filepath = program.get<std::string>("--output-junit");
  if (!junit_output_filepath.empty()) {
    auto* file_stream = new std::ofstream(junit_output_filepath);
    auto junit_output = std::make_shared<Formatters::JUnitXML>(*file_stream, false);
    return Runner{formatter, junit_output};
  }

  return Runner{formatter};
}
}  // namespace CppSpec

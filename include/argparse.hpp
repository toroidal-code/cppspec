#pragma once

#include <argparse/argparse.hpp>
#include <fstream>
#include <memory>
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

/**
 * @brief The file that --output-junit names, with the JUnit XML formatter that writes to it
 *
 * The formatter is declared after the stream, so it is destroyed first: its destructor writes the XML to a stream
 * that is still open, and the stream is closed after it.
 */
struct JUnitOutputFile {
  std::ofstream stream;
  Formatters::JUnitXML formatter{stream, false};

  explicit JUnitOutputFile(const std::string& path) : stream(path) {}
};

inline Runner parse(int argc, char** const argv) {
  std::filesystem::path executable_path = argv[0];
  std::string executable_name = executable_path.filename().string();
  argparse::ArgumentParser program{executable_name};

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
    auto junit_file = std::make_shared<JUnitOutputFile>(junit_output_filepath);
    std::shared_ptr<Formatters::BaseFormatter> junit_output{junit_file, &junit_file->formatter};
    return Runner{formatter, junit_output};
  }
  return Runner{formatter};
}
}  // namespace CppSpec

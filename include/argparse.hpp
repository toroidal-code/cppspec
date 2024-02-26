#pragma once

#include <argparse/argparse.hpp>
#include <string_view>

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

struct RuntimeOpts {
  bool verbose = false;
  std::shared_ptr<Formatters::BaseFormatter> formatter = nullptr;
};

inline Runner parse(int argc, char** const argv) {
  argparse::ArgumentParser program{file_name(argv[0])};

  program.add_argument("-f", "--format")
      .default_value(std::string{"p"})
      .choices("progress", "p", "tap", "t", "detail", "d")
      .required()
      .help("set the output format");

  program.add_argument("--verbose").help("increase output verbosity").default_value(false).implicit_value(true);

  try {
    program.parse_args(argc, argv);
  } catch (const std::runtime_error& err) {
    std::cerr << err.what() << std::endl;
    std::cerr << program;
    std::exit(1);
  }

  RuntimeOpts opts;

  auto format_string = program.get<std::string>("--format");
  if (format_string == "d" || format_string == "detail" || program["--verbose"] == true) {
    opts.formatter = std::make_unique<Formatters::Verbose>();
  } else if (format_string == "p" || format_string == "progress") {
    opts.formatter = std::make_unique<Formatters::Progress>();
  } else if (format_string == "t" || format_string == "tap") {
    opts.formatter = std::make_unique<Formatters::TAP>();
  } else {
    std::cerr << "Unrecognized format type" << std::endl;
    std::exit(-1);
  }

  return Runner{opts.formatter};
}
}  // namespace CppSpec

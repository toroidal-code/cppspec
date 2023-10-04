#pragma once

#include <argparse/argparse.hpp>
#include <string_view>

#include "argparse.hpp"
#include "formatters/formatters_base.hpp"

namespace CppSpec {
constexpr std::string file_name(std::string_view path) {
  std::string_view file = path;
  for (size_t i = 0; i < path.size(); ++i) {
    if (path[i] == '/') {
      file = &path[i + 1];
    }
  }
  return std::string{file};
}

enum class Formatter {
  Progress,
  TAP,
  Detail
};

struct RuntimeOpts {
  bool verbose = false;
  Formatter format = Formatter::Progress;
};

inline RuntimeOpts parse(int argc, char** argv) {
  argparse::ArgumentParser program(file_name(__FILE__));

  program.add_argument("-f", "--format")
      .default_value(std::string{"p"})
      .required()
      .help("set the output format");

  program.add_argument("--verbose")
      .help("increase output verbosity")
      .default_value(false)
      .implicit_value(true);

  try {
    program.parse_args(argc, argv);
  } catch (const std::runtime_error& err) {
    std::cerr << err.what() << std::endl;
    std::cerr << program;
    std::exit(1);
  }

  RuntimeOpts opts;

  if (program["--verbose"] == true) {
    opts.verbose = true;
  }

  auto format_string  = program.get<std::string>("--format");
  if (format_string == "p" || format_string == "progress") {
    opts.format = Formatter::Progress;
  } else if (format_string == "t" || format_string == "tap") {
    opts.format = Formatter::TAP;
  } else if (format_string == "d" || format_string == "detail") {
    opts.format = Formatter::Detail;
  }
}
}  // namespace CppSpec
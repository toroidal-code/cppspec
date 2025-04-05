/** @file */
#pragma once

namespace CppSpec {

// the following are Unix/BASH ONLY terminal color codes.
constexpr auto RESET("\033[0m");
constexpr auto BLACK("\033[30m");              /* Black */
constexpr auto RED("\033[31m");                /* Red */
constexpr auto GREEN("\033[32m");              /* Green */
constexpr auto YELLOW("\033[33m");             /* Yellow */
constexpr auto BLUE("\033[34m");               /* Blue */
constexpr auto MAGENTA("\033[35m");            /* Magenta */
constexpr auto CYAN("\033[36m");               /* Cyan */
constexpr auto WHITE("\033[37m");              /* White */
constexpr auto BOLDBLACK("\033[1m\033[30m");   /* Bold Black */
constexpr auto BOLDRED("\033[1m\033[31m");     /* Bold Red */
constexpr auto BOLDGREEN("\033[1m\033[32m");   /* Bold Green */
constexpr auto BOLDYELLOW("\033[1m\033[33m");  /* Bold Yellow */
constexpr auto BOLDBLUE("\033[1m\033[34m");    /* Bold Blue */
constexpr auto BOLDMAGENTA("\033[1m\033[35m"); /* Bold Magenta */
constexpr auto BOLDCYAN("\033[1m\033[36m");    /* Bold Cyan */
constexpr auto BOLDWHITE("\033[1m\033[37m");   /* Bold White */

}  // namespace CppSpec

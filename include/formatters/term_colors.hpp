/** @file */
#pragma once
#include <string_view>

// the following are Unix/BASH ONLY terminal color codes.
constexpr std::string_view RESET("\033[0m");
constexpr std::string_view BLACK("\033[30m");              /* Black */
constexpr std::string_view RED("\033[31m");                /* Red */
constexpr std::string_view GREEN("\033[32m");              /* Green */
constexpr std::string_view YELLOW("\033[33m");             /* Yellow */
constexpr std::string_view BLUE("\033[34m");               /* Blue */
constexpr std::string_view MAGENTA("\033[35m");            /* Magenta */
constexpr std::string_view CYAN("\033[36m");               /* Cyan */
constexpr std::string_view WHITE("\033[37m");              /* White */
constexpr std::string_view BOLDBLACK("\033[1m\033[30m");   /* Bold Black */
constexpr std::string_view BOLDRED("\033[1m\033[31m");     /* Bold Red */
constexpr std::string_view BOLDGREEN("\033[1m\033[32m");   /* Bold Green */
constexpr std::string_view BOLDYELLOW("\033[1m\033[33m");  /* Bold Yellow */
constexpr std::string_view BOLDBLUE("\033[1m\033[34m");    /* Bold Blue */
constexpr std::string_view BOLDMAGENTA("\033[1m\033[35m"); /* Bold Magenta */
constexpr std::string_view BOLDCYAN("\033[1m\033[36m");    /* Bold Cyan */
constexpr std::string_view BOLDWHITE("\033[1m\033[37m");   /* Bold White */

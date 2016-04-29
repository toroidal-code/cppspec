/** @file */
#ifndef CPPSPEC_RESULT_HPP
#define CPPSPEC_RESULT_HPP
#pragma once

#include <string>
#include <sstream>
#include <ciso646>

namespace CppSpec {

class Result {
  const bool value;
  std::string message;
  explicit Result(bool value, std::string message = "") noexcept
      : value(value),
        message(message) {}

 public:
  // Default destructor
  virtual ~Result() = default;

  // Copy constructor/operator
  Result(const Result &) = default;
  Result &operator=(const Result &) = default;

  // Move constructor/operator
  Result(Result &&) = default;
  Result &operator=(Result &&) = default;

  /*--------- Status helper functions --------------*/
  constexpr const bool get_status() noexcept { return value; }
  constexpr const bool get_status() const noexcept { return value; }

  constexpr operator bool() noexcept { return this->get_status(); }
  constexpr operator bool() const noexcept { return this->get_status(); }

  /*--------- Message helper functions -------------*/
  const std::string get_message() noexcept { return message; }
  const std::string get_message() const noexcept { return message; }
  Result &set_message(std::string message) noexcept;

  /*--------- Explicit constructor functions -------*/
  static Result success() noexcept;
  static Result failure() noexcept;
  static Result success_with(std::string success_message) noexcept;
  static Result failure_with(std::string failure_message) noexcept;

  /*-------------- Friend functions ----------------*/

  // Stream operator
  friend std::ostream &operator<<(std::ostream &os, const Result &res);
};

inline Result &Result::set_message(std::string message) noexcept {
  this->message = message;
  return *this;
}

inline Result Result::success() noexcept { return Result(true); }
inline Result Result::success_with(std::string success_message) noexcept {
  return Result(true, success_message);
}

inline Result Result::failure() noexcept { return Result(false); }
inline Result Result::failure_with(std::string failure_message) noexcept {
  return Result(false, failure_message);
}

inline std::ostream &operator<<(std::ostream &os, const Result &res) {
  std::stringstream ss;
  ss << (res.get_status() ? "Result::success" : "Result::failure");

  if (not res.get_message().empty()) {
    ss << "(\"" + res.get_message() + "\")";
  }

  return os << ss.str();
}

}  // namespace CppSpec
#endif  // CPPSPEC_RESULT_HPP

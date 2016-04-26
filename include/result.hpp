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
  explicit Result(bool value, std::string message = "")
      : value(value), message(message) {}

 public:
  // Default destructor
  virtual ~Result() = default;

  // Move constructor/operator
  Result(Result &&) = default;
  Result &operator=(Result &&) = default;

  // Copy constructor/operator
  Result(const Result &) = default;
  Result &operator=(const Result &) = default;

  /*--------- Message helper functions -------------*/
  const std::string get_message() { return message; }
  const std::string get_message() const { return message; }
  Result &set_message(std::string message);

  /*--------- Status helper functions --------------*/
  const bool get_status() { return value; }
  const bool get_status() const { return value; }

  operator bool() { return this->get_status(); }

  /*--------- Explicit constructor functions -------*/
  static Result success();
  static Result failure();
  static Result success_with(std::string success_message);
  static Result failure_with(std::string failure_message);

  /*-------------- Friend functions ----------------*/

  // Stream operator
  friend std::ostream &operator<<(std::ostream &os, const Result &res);
};

inline Result &Result::set_message(std::string message) {
  this->message = message;
  return *this;
}

inline Result Result::success() { return Result(true); }
inline Result Result::success_with(std::string success_message) {
  return Result(true, success_message);
}

inline Result Result::failure() { return Result(false); }
inline Result Result::failure_with(std::string failure_message) {
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

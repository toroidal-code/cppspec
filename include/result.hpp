/** @file */
#pragma once

#include <ciso646>
#include <sstream>
#include <string>
#include <utility>

namespace CppSpec {

class Result {
  const bool value;
  std::string message;
  explicit Result(bool value, std::string message = "") noexcept : value(value), message(std::move(message)) {}

 public:
  // Default destructor
  virtual ~Result() = default;

  // Copy constructor/operator
  Result(const Result &) = default;
  Result &operator=(const Result &) = delete;

  // Move constructor/operator
  Result(Result &&) = default;
  Result &operator=(Result &&) = delete;

  /*--------- Status helper functions --------------*/
  [[nodiscard]] bool get_status() const noexcept { return value; }

  operator bool() const noexcept { return this->get_status(); }

  /*--------- Message helper functions -------------*/
  std::string get_message() noexcept { return message; }
  [[nodiscard]] std::string get_message() const noexcept { return message; }
  Result &set_message(const std::string &message) noexcept;

  /*--------- Explicit constructor functions -------*/
  static Result success() noexcept;
  static Result failure() noexcept;
  static Result success_with(const std::string &success_message) noexcept;
  static Result failure_with(const std::string &failure_message) noexcept;

  /*-------------- Friend functions ----------------*/

  // Stream operator
  friend std::ostream &operator<<(std::ostream &os, const Result &res);
};

inline Result &Result::set_message(const std::string &message) noexcept {
  this->message = message;
  return *this;
}

inline Result Result::success() noexcept { return Result(true); }
inline Result Result::success_with(const std::string &success_message) noexcept {
  return Result(true, success_message);
}

inline Result Result::failure() noexcept { return Result(false); }
inline Result Result::failure_with(const std::string &failure_message) noexcept {
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

template <typename T>
constexpr bool is_result_v = std::is_same_v<Result,T>;

template <typename T>
concept is_result = is_result_v<T>;

}  // namespace CppSpec

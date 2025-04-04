/** @file */
#pragma once

#include <ciso646>
#include <format>
#include <source_location>
#include <sstream>
#include <string>

namespace CppSpec {

class Result {
  bool value = false;
  std::source_location location;
  std::string message;
  std::string type;
  explicit Result(bool value, std::source_location location, const std::string& message = "") noexcept
      : value(value), location(location), message(message) {}

 public:
  Result() = default;

  /*--------- Status helper functions --------------*/
  [[nodiscard]] bool status() const noexcept { return value; }
  [[nodiscard]] bool is_success() const noexcept { return value; }
  [[nodiscard]] bool is_failure() const noexcept { return !value; }

  /*--------- Location helper functions ------------*/
  [[nodiscard]] std::source_location get_location() const noexcept { return location; }
  [[nodiscard]] std::string get_location_string() const noexcept {
    return std::format("{}:{}:{}", location.file_name(), location.line(), location.column());
  }

  [[nodiscard]] std::string get_type() const noexcept { return type; }
  [[nodiscard]] std::string get_type() noexcept { return type; }
  void set_type(const std::string& type) noexcept { this->type = type; }

  /*--------- Message helper functions -------------*/
  std::string get_message() noexcept { return message; }
  [[nodiscard]] std::string get_message() const noexcept { return message; }
  Result& set_message(const std::string& message) noexcept;

  /*--------- Explicit constructor functions -------*/
  static Result success(std::source_location location) noexcept;
  static Result failure(std::source_location location) noexcept;
  static Result success_with(std::source_location location, const std::string& success_message) noexcept;
  static Result failure_with(std::source_location location, const std::string& failure_message) noexcept;

  /*-------------- Friend functions ----------------*/

  // Stream operator
  friend std::ostream& operator<<(std::ostream& os, const Result& res);

  friend Result& operator&=(Result& lhs, const Result& rhs) {
    lhs.value = lhs.value && rhs.value;
    return lhs;
  }

  friend Result operator&&(const Result& lhs, const Result& rhs) {
    Result result;
    result.location = lhs.location;
    result.message = lhs.message + " and\n" + rhs.message;
    result.value = lhs.value && rhs.value;
    return result;
  }
};

inline Result& Result::set_message(const std::string& message) noexcept {
  this->message = message;
  return *this;
}

inline Result Result::success(std::source_location location) noexcept {
  return Result(true, location);
}
inline Result Result::success_with(std::source_location location, const std::string& success_message) noexcept {
  return Result(true, location, success_message);
}

inline Result Result::failure(std::source_location location) noexcept {
  return Result(false, location);
}
inline Result Result::failure_with(std::source_location location, const std::string& failure_message) noexcept {
  return Result(false, location, failure_message);
}

inline std::ostream& operator<<(std::ostream& os, const Result& res) {
  std::stringstream ss;
  ss << (res.status() ? "Result::success" : "Result::failure");

  if (not res.get_message().empty()) {
    ss << "(\"" + res.get_message() + "\")";
  }

  return os << ss.str();
}

template <typename T>
constexpr bool is_result_v = std::is_same_v<Result, T>;

template <typename T>
concept is_result = is_result_v<T>;

}  // namespace CppSpec

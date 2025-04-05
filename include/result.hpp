/** @file */
#pragma once

#include <ciso646>
#include <format>
#include <source_location>
#include <sstream>
#include <string>
#include <utility>

namespace CppSpec {

class Result {
 public:
  enum class Status { Success, Failure, Error, Skipped };

  Result() = default;

  /*--------- Status helper functions --------------*/
  void set_status(Status status) noexcept { status_ = status; }
  [[nodiscard]] Status status() const noexcept { return status_; }
  [[nodiscard]] bool is_success() const noexcept { return status_ == Status::Success; }
  [[nodiscard]] bool is_failure() const noexcept { return status_ == Status::Failure; }
  [[nodiscard]] bool skipped() const noexcept { return status_ == Status::Skipped; }
  [[nodiscard]] bool is_error() const noexcept { return status_ == Status::Error; }

  static Result reduce(const Result& lhs, const Result& rhs) noexcept {
    if (lhs.is_failure()) {
      return lhs;
    } else if (rhs.is_failure()) {
      return rhs;
    } else if (lhs.is_success()) {
      return lhs;
    } else if (rhs.is_success()) {
      return rhs;
    } else {
      return lhs;
    }
  }

  /*--------- Location helper functions ------------*/
  [[nodiscard]] std::source_location get_location() const noexcept { return location; }
  [[nodiscard]] std::string get_location_string() const noexcept {
    return std::format("{}:{}:{}", location.file_name(), location.line(), location.column());
  }

  [[nodiscard]] std::string get_type() const noexcept { return type; }
  [[nodiscard]] std::string get_type() noexcept { return type; }
  void set_type(std::string type) noexcept { this->type = std::move(type); }

  /*--------- Message helper functions -------------*/
  std::string get_message() noexcept { return message; }
  [[nodiscard]] std::string get_message() const noexcept { return message; }
  Result& set_message(std::string message) noexcept {
    this->message = std::move(message);
    return *this;
  }

  /*--------- Explicit constructor functions -------*/
  static Result success(std::source_location location) noexcept { return {Status::Success, location}; }
  static Result failure(std::source_location location) noexcept { return {Status::Failure, location}; }
  static Result success_with(std::source_location location, std::string success_message) noexcept {
    return {Status::Success, location, std::move(success_message)};
  }
  static Result failure_with(std::source_location location, std::string failure_message) noexcept {
    return {Status::Failure, location, std::move(failure_message)};
  }

  static Result error(std::source_location location) noexcept { return {Status::Error, location}; }
  static Result error_with(std::source_location location, std::string error_message) noexcept {
    return {Status::Error, location, std::move(error_message)};
  }

  static Result skipped(std::source_location location) noexcept { return {Status::Skipped, location}; }
  static Result skipped_with(std::source_location location, std::string skipped_message) noexcept {
    return {Status::Skipped, location, std::move(skipped_message)};
  }

  /*-------------- Friend functions ----------------*/

  // Stream operator
  friend std::ostream& operator<<(std::ostream& os, const Result& res) {
    std::stringstream ss;
    switch (res.status()) {
      case Status::Success:
        ss << "Success";
        break;
      case Status::Failure:
        ss << "Failure";
        break;
      case Status::Error:
        ss << "Error";
        break;
      case Status::Skipped:
        ss << "Skipped";
        break;
    }

    if (not res.get_message().empty()) {
      ss << "(\"" + res.get_message() + "\")";
    }

    return os << ss.str();
  }

 private:
  Status status_ = Status::Success;
  std::source_location location;
  std::string message;
  std::string type;
  Result(Status status, std::source_location location, std::string message = "") noexcept
      : status_(status), location(location), message(std::move(message)) {}
};

template <typename T>
constexpr bool is_result_v = std::is_same_v<Result, T>;

template <typename T>
concept is_result = is_result_v<T>;

}  // namespace CppSpec

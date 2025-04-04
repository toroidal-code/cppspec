/**
 * @file
 * @brief Utility functions and classes
 */
#pragma once
#include <functional>
#include <ranges>
#include <sstream>
#include <string>
#include <type_traits>

#ifdef __GNUG__
#include <cxxabi.h>

#include <memory>

#endif

namespace CppSpec::Util {

/**
 * Demangle a symbol into a human-readable string.
 *
 * @param mangled the mangled symbol
 *
 * @return a human-readable translation of the given symbol
 */
#ifdef __GNUG__
inline std::string demangle(const char* mangled) {
  int status;
  std::unique_ptr<char[], void (*)(void*)> result{
      abi::__cxa_demangle(mangled, nullptr, nullptr, &status),
      std::free,
  };
  return (status == 0) ? result.get() : mangled;
}
#else
inline std::string demangle(const char* name) {
  return name;
}
#endif

/**
 * @brief Helper class for static assertions that has a built-in error string.
 */
template <bool result>
struct verbose_assert {
  static_assert(result, "Assertion failed (see below for more information)");
  static bool const value = result;
};

/**
 * @brief Concept for checking whether T is a container type.
 *
 * Provides the member constant value which is equal
 * to `true`, if T is a container type. Otherwise,
 * value is equal to `false`.
 *
 * A container is defined by having the functions
 * `begin`, `end`, `max_size`, and `empty`.
 *
 * @tparam T a type to check
 */
template <typename C>
concept is_container = requires(C c) {
  requires std::ranges::range<C>;
  { c.max_size() } -> std::integral;
  { c.empty() } -> std::convertible_to<bool>;
};

/**
 * @brief Checks whether T can be streamed.
 *
 *
 * A streamable type is defined by having the operator<<
 * defined in scope.
 *
 * @tparam C a type to check
 */
template <typename C>
concept is_streamable = requires(std::ostream& os, const C& obj) {
  { os << obj } -> std::same_as<std::ostream&>;
};

template <typename C>
concept is_functional =
    // The C++ reference says that all "functions" (not FunctionObjects) respond to
    // std::is_function<T>
    std::is_function_v<C> ||

    // An alternative from Stack Overflow question 18107368. Anything that could
    // possibly be "functional" should be able to be cast to std::function<void()>
    std::is_convertible_v<C, std::function<void()>> ||

    // The C++ reference also says that something qualifies as a FunctionObject iff
    // std::is_object_v<T> is true and the object responds to the call operator()
    requires(C func) {
      requires std::is_object_v<C>;
      { func() } -> std::invocable<>;
    };

template <typename C>
concept is_not_functional = !is_functional<C>;

template <typename T>
concept not_c_string = !std::is_same_v<T, const char*> && !std::is_same_v<T, char*> &&
                       !std::is_convertible_v<T, const char*> && !std::is_convertible_v<T, char*>;

/**
 * @brief Implode a string
 *
 * @tparam Range the type of the iterable object. is_iterable<Range>::value
 *         must evaluate to true.
 * @param iterable an iterable object
 * @param separator a user-defined separator to delimit elements
 *
 * @return the joined string
 */
[[nodiscard]] inline std::string join(std::ranges::range auto& iterable, const std::string& separator = "") {
  std::ostringstream oss;
  bool first = true;
  for (auto& thing : iterable) {
    if (!first) {
      oss << separator;
    }
    oss << thing;  // use operator<< to convert and append
    first = false;
  }
  return oss.str();
}

}  // namespace CppSpec::Util

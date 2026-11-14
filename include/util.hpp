/**
 * @file
 * @brief Utility functions and classes
 */
#pragma once
#include <algorithm>
#include <concepts>
#include <functional>
#include <optional>
#include <ranges>
#include <sstream>
#include <string>
#include <string_view>
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

/**
 * @brief Checks whether T is a C string: a pointer to char, const or not, or a char array, which decays to one.
 *
 * A pointer to volatile char is not a C string: it is printed and compared as a plain pointer.
 *
 * @tparam T a type to check
 */
template <typename T>
concept is_c_string = std::is_pointer_v<std::decay_t<T>> &&
                      std::same_as<std::remove_const_t<std::remove_pointer_t<std::decay_t<T>>>, char>;

/**
 * @brief Checks whether T is a character type that std::ostream does not print as a character: wchar_t, char8_t,
 *        char16_t or char32_t.
 *
 * @tparam T a type to check
 */
template <typename T>
concept is_wide_character = std::same_as<std::remove_cv_t<T>, wchar_t> || std::same_as<std::remove_cv_t<T>, char8_t> ||
                            std::same_as<std::remove_cv_t<T>, char16_t> || std::same_as<std::remove_cv_t<T>, char32_t>;

/**
 * @brief Checks whether T is a pointer to a character type other than char: signed char, unsigned char, or a wide
 *        character type.
 *
 * Only a char pointer is a C string. These point at bytes or code units, so they are printed as addresses and
 * compared as pointers.
 *
 * @tparam T a type to check
 */
template <typename T>
concept is_non_text_character_pointer =
    std::is_pointer_v<T> && (std::same_as<std::remove_const_t<std::remove_pointer_t<T>>, signed char> ||
                             std::same_as<std::remove_const_t<std::remove_pointer_t<T>>, unsigned char> ||
                             std::same_as<std::remove_const_t<std::remove_pointer_t<T>>, wchar_t> ||
                             std::same_as<std::remove_const_t<std::remove_pointer_t<T>>, char8_t> ||
                             std::same_as<std::remove_const_t<std::remove_pointer_t<T>>, char16_t> ||
                             std::same_as<std::remove_const_t<std::remove_pointer_t<T>>, char32_t>);

/**
 * @brief Checks whether T is a std::optional.
 *
 * @tparam T a type to check
 */
template <typename T>
inline constexpr bool is_optional_v = false;

template <typename T>
inline constexpr bool is_optional_v<std::optional<T>> = true;

/**
 * @brief Checks whether T is text that CppSpec::string_view_of() can view: a C string, or a type that converts to
 *        std::string_view, such as std::string and std::string_view.
 *
 * @tparam T a type to check
 */
template <typename T>
concept is_string_like = is_c_string<T> || std::convertible_to<const std::remove_cvref_t<T>&, std::string_view>;

/**
 * @brief Checks whether to_start_with and to_end_with compare an actual value of type A with an expected one of type
 *        E: text with text, a range with a range element by element, or a range of char with text.
 *
 * @tparam A the type of the actual value
 * @tparam E the type of the expected value
 */
template <typename A, typename E>
concept compares_start_and_end =
    (is_string_like<A> && is_string_like<E>) ||
    (std::ranges::range<A> &&
     (std::ranges::range<E> ||
      (is_string_like<E> && std::same_as<std::remove_cv_t<std::ranges::range_value_t<A>>, char>)));

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

[[nodiscard]] inline std::string join_endl(std::ranges::range auto&& iterable) {
  std::ostringstream oss;
  bool first = true;
  for (auto& thing : iterable) {
    if (!first) {
      oss << std::endl;
    }
    oss << thing;  // use operator<< to convert and append
    first = false;
  }
  return oss.str();
}

}  // namespace CppSpec::Util

namespace CppSpec {

/**
 * @brief View text as a std::string_view, the one form the string matchers and the printer compare and print.
 *
 * A char array passed here itself is read up to its first NUL and no further than its end. A char array given to
 * expect() or to a matcher has already decayed to a pointer by the time it is viewed, so it is read as a C string,
 * up to its first NUL, and must be NUL-terminated; wrap a fixed-size buffer in `std::string_view(buffer, size)`.
 *
 * A null C string is no text at all: it equals only another null C string, the matchers that search text fail on
 * it, negated or not, whether it is the actual value or the text looked for, and it prints as `nullptr`.
 *
 * A temporary std::string is refused, because the view would outlive its text.
 *
 * @param text a std::string, std::string_view, char array or char pointer
 *
 * @return a view of the text, or std::nullopt for a null C string
 */
template <Util::is_string_like T>
[[nodiscard]] constexpr std::optional<std::string_view> string_view_of(const T& text) {
  if constexpr (std::is_array_v<T>) {
    return std::string_view(std::ranges::begin(text), std::ranges::find(text, '\0'));
  } else if constexpr (Util::is_c_string<T>) {
    if (text == nullptr) {
      return std::nullopt;
    }
    return std::string_view(text);
  } else {
    return std::string_view(text);
  }
}

std::optional<std::string_view> string_view_of(const std::string&& text) = delete;

}  // namespace CppSpec

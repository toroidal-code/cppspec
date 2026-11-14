/** @file */
#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <format>
#include <forward_list>
#include <list>
#include <map>
#include <optional>
#include <ranges>
#include <regex>
#include <set>
#include <span>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "prettyprint.hpp"
#include "util.hpp"

// C++26 (P3168) gives std::optional begin()/end(), so cxx-prettyprint's
// has_begin_end/has_const_iterator detection now classifies optional<T> as a
// container and tries to element-print its value_type — which hard-errors for
// element types lacking operator<< (and is wrong regardless: an optional is not
// a container). Exclude it from prettyprint's container detection so optionals
// reach Pretty's own optional to_word overload.
namespace pretty_print {
template <typename T>
struct is_container<std::optional<T>> : std::false_type {};
} // namespace pretty_print

namespace CppSpec {

namespace Util {

// Whether T is Template<Args...> for some type arguments; std::array and std::span, which take a size, have their own.
template <typename T, template <typename...> class Template>
inline constexpr bool is_specialization_of = false;

template <template <typename...> class Template, typename... Args>
inline constexpr bool is_specialization_of<Template<Args...>, Template> = true;

template <typename T>
inline constexpr bool is_std_array = false;

template <typename T, std::size_t N>
inline constexpr bool is_std_array<std::array<T, N>> = true;

template <typename T>
inline constexpr bool is_std_span = false;

template <typename T, std::size_t Extent>
inline constexpr bool is_std_span<std::span<T, Extent>> = true;

/**
 * @brief Checks whether T is a standard container, a std::span, a std::pair, a std::tuple or a built-in array: a type
 *        whose operator<< is cxx-prettyprint's, so Pretty may print it element by element in its place.
 *
 * @tparam T a type to check, without cv-qualifiers
 */
template <typename T>
concept is_std_container_like =
    std::is_array_v<T> || is_std_array<T> || is_std_span<T> || is_specialization_of<T, std::vector> ||
    is_specialization_of<T, std::deque> || is_specialization_of<T, std::list> ||
    is_specialization_of<T, std::forward_list> || is_specialization_of<T, std::set> ||
    is_specialization_of<T, std::multiset> || is_specialization_of<T, std::unordered_set> ||
    is_specialization_of<T, std::unordered_multiset> || is_specialization_of<T, std::map> ||
    is_specialization_of<T, std::multimap> || is_specialization_of<T, std::unordered_map> ||
    is_specialization_of<T, std::unordered_multimap> || is_specialization_of<T, std::pair> ||
    is_specialization_of<T, std::tuple>;

/**
 * @brief Whether T prints differently through Pretty than through operator<<, so a container holding it is printed
 *        element by element through Pretty.
 *
 * True for a bool, a byte-sized integer, a wide character, a C string, a pointer to other characters, an enum with no
 * operator<< and an optional, and for a pair, a tuple, a standard container, a built-in array or a range with no
 * operator<< of its own that holds one of these, however deeply. A range type with its own operator<< prints through
 * it.
 *
 * @tparam T a type to check, without cv-qualifiers
 */
template <typename T>
struct prints_through_pretty
    : std::bool_constant<std::same_as<T, bool> || std::same_as<T, signed char> || std::same_as<T, unsigned char> ||
                         is_wide_character<T> || is_c_string<T> || is_non_text_character_pointer<T> ||
                         (std::is_enum_v<T> && !is_streamable<T>)> {};

template <typename T>
struct prints_through_pretty<std::optional<T>> : std::true_type {};

template <typename A, typename B>
struct prints_through_pretty<std::pair<A, B>> : std::bool_constant<prints_through_pretty<std::remove_cv_t<A>>::value ||
                                                                   prints_through_pretty<std::remove_cv_t<B>>::value> {
};

template <typename... Ts>
struct prints_through_pretty<std::tuple<Ts...>>
    : std::bool_constant<(prints_through_pretty<std::remove_cv_t<Ts>>::value || ...)> {};

// A range whose elements are of its own type, such as std::filesystem::path, is printed whole, and so is a range type
// with an operator<< of its own.
template <typename R>
  requires std::ranges::input_range<const R> && (!is_string_like<R>) &&
           (!std::same_as<std::remove_cv_t<std::ranges::range_value_t<const R>>, R>) &&
           (is_std_container_like<R> || !is_streamable<R>)
struct prints_through_pretty<R> : prints_through_pretty<std::remove_cv_t<std::ranges::range_value_t<const R>>> {};

/**
 * @brief Checks whether T is a container, pair or tuple that Pretty prints element by element, because an element
 *        prints differently through Pretty than through operator<<.
 *
 * @tparam T a type to check
 */
template <typename T>
concept prints_elements_through_pretty =
    (std::is_class_v<T> || std::is_array_v<T>) && !is_string_like<T> && !is_optional_v<std::remove_cv_t<T>> &&
    prints_through_pretty<std::remove_cv_t<T>>::value;

}  // namespace Util

namespace Matchers {
template <typename A, typename E>
class MatcherBase;
}

/**
 * @class Pretty
 * @brief A helper base class that assists in pretty-printing various objects.
 *
 * This class provides helper string manipulation functions
 * that are used to generate human-readable information and output.
 */
struct Pretty {
  std::string _name;
  [[nodiscard]] std::string name(const std::string& name) const;
  [[nodiscard]] std::string name_to_sentence(const std::string& name) const;
  static std::string split_words(const std::string& sym);
  static std::string underscore(const std::string& camel_cased_word);
  static std::string last(const std::string& s, char delim);
  static std::string improve_hash_formatting(const std::string& inspect_string);

  template <typename T>
    requires Util::is_streamable<T> && (!Util::prints_elements_through_pretty<T>) &&
             (!Util::is_non_text_character_pointer<T>)
  static std::string to_word(const T& item);

  template <typename T>
  static std::string to_word(const T& item);

  template <typename T>
    requires Util::is_wide_character<T>
  static std::string to_word(const T& item);

  template <typename T>
    requires Util::is_non_text_character_pointer<T>
  static std::string to_word(const T& item);

  template <typename T>
    requires Util::prints_elements_through_pretty<T>
  static std::string to_word(const T& item);

  template <typename T>
    requires std::is_enum_v<T> && (!Util::is_streamable<T>)
  static std::string to_word(const T& item);

  template <typename T>
  static std::string to_word(const std::optional<T>& item);

  template <typename T>
  static std::string to_word_type(const T& item);

  template <typename A, typename E>
  static std::string to_word(const Matchers::MatcherBase<A, E>& matcher);

  template <class T>
  static std::string to_sentence(const T& item);

  template <class T>
  static std::string to_sentence(const std::vector<T>& words);

  template <typename T>
  static std::string inspect_object(const T& object);
};

/**
 * Takes a vector of objects and formats them as an
 * English style list (i.e. "a, b, and c")
 *
 * @param words the collection of objects to be processed
 *
 * @return a human-readable comma-delimited list
 */
template <typename T>
inline std::string Pretty::to_sentence(const std::vector<T>& objects) {
  std::vector<std::string> words;
  for (const auto& object : objects) {
    words.push_back(to_word(object));
  }

  std::stringstream ss;
  switch (words.size()) {
    case 0:
      return "";
    case 1:
      ss << " " << words[0];
      break;
    case 2:
      ss << " " << words[0] << " and " << words[1];
      break;
    default:
      ss << " ";
      for (size_t i = 0; i < words.size() - 1; ++i) {
        if (i != 0) {
          ss << ", ";
        }
        ss << words[i];
      }
      ss << ", and " << words.back();
      break;
  }

  return ss.str();
}

/**
 * @brief Take a single object and format it as a sentance
 *
 * @param item the object to be processed
 *
 * @return a human-readable representation of the object (as a list)
 */
template <typename T>
inline std::string Pretty::to_sentence(const T& item) {
  return to_sentence(std::vector{item});
}

/**
 * @brief Formats an object as a string when operator<< is available
 *
 * In reality, this uses the `<<` operator, simply
 * providing a way to convert the object to a string without
 * having to deal with stringstreams.
 *
 * @param item the object to be processed
 *
 * @return the string representation
 */
template <typename T>
  requires Util::is_streamable<T> && (!Util::prints_elements_through_pretty<T>) &&
           (!Util::is_non_text_character_pointer<T>)
inline std::string Pretty::to_word(const T& item) {
  if constexpr (Util::is_c_string<T>) {
    // A C string is printed through its view, so a null one prints as nullptr instead of being read.
    const auto text = string_view_of(item);
    return text ? std::string(*text) : std::string("nullptr");
  } else {
    std::ostringstream oss;
    oss << item;  // use operator<< to convert and append
    return oss.str();
  }
}

template <>
inline std::string Pretty::to_word<bool>(const bool& item) {
  return item ? "true" : "false";
}

// A byte-sized integer streams as a character; it is printed as the number it holds.
template <>
inline std::string Pretty::to_word<unsigned char>(const unsigned char& item) {
  return std::to_string(item);
}

template <>
inline std::string Pretty::to_word<signed char>(const signed char& item) {
  return std::to_string(item);
}

template <>
inline std::string Pretty::to_word<std::true_type>(const std::true_type& /* item */) {
  return "true";
}

template <>
inline std::string Pretty::to_word<std::false_type>(const std::false_type& /* item */) {
  return "false";
}

/**
 * @brief Formats an object as a string when there is no operator<<
 *
 * @param item the object to be processed
 *
 * @return the string representation
 */
template <typename T>
inline std::string Pretty::to_word(const T& item) {
  std::stringstream ss;
  // Ruby-style inspect for objects without an overloaded operator<<
  ss << "#<" << Util::demangle(typeid(item).name()) << ":" << &item << ">";
  return ss.str();
}

/**
 * @brief Formats a wide character (wchar_t, char8_t, char16_t or char32_t) as the number of its code unit
 *
 * std::ostream has no operator<< for these, so they are printed as numbers, like byte-sized integers.
 *
 * @param item the character to be processed
 *
 * @return the string representation, e.g. `97` for `u'a'`
 */
template <typename T>
  requires Util::is_wide_character<T>
inline std::string Pretty::to_word(const T& item) {
  return std::to_string(static_cast<std::uint_least32_t>(item));
}

/**
 * @brief Formats a pointer to signed char, unsigned char or a wide character as an address, or `nullptr`
 *
 * Only a char pointer is read as text. These point at bytes or code units, which std::ostream would otherwise read
 * up to a NUL (signed and unsigned char) or not print at all (wide characters).
 *
 * @param item the pointer to be processed
 *
 * @return the string representation
 */
template <typename T>
  requires Util::is_non_text_character_pointer<T>
inline std::string Pretty::to_word(const T& item) {
  if (item == nullptr) {
    return "nullptr";
  }
  std::ostringstream oss;
  oss << static_cast<const void*>(item);
  return oss.str();
}

/**
 * @brief Formats a container, pair or tuple whose elements Pretty prints differently from operator<<, element by
 *        element through to_word()
 *
 * The container keeps cxx-prettyprint's delimiters (`[1, 2]`, `{1, 2}` for a set, `(1, 2)` for a pair or tuple), so
 * only the elements read differently: `std::vector<std::uint8_t>{65, 66}` prints `[65, 66]`, not `[A, B]`.
 *
 * @param item the container, pair or tuple to be processed
 *
 * @return the string representation
 */
template <typename T>
  requires Util::prints_elements_through_pretty<T>
inline std::string Pretty::to_word(const T& item) {
  const auto& delimiters = pretty_print::delimiters<T, char>::values;
  std::string word = delimiters.prefix != nullptr ? delimiters.prefix : "";
  bool first = true;
  auto append = [&](const auto& element) {
    if (!first && delimiters.delimiter != nullptr) {
      word += delimiters.delimiter;
    }
    word += to_word(element);
    first = false;
  };
  if constexpr (std::ranges::input_range<const T>) {
    for (const auto& element : item) {
      append(element);
    }
  } else {
    std::apply([&](const auto&... elements) { (append(elements), ...); }, item);
  }
  if (delimiters.postfix != nullptr) {
    word += delimiters.postfix;
  }
  return word;
}

/**
 * @brief Formats an enum that has no operator<< as its type and underlying value as a number, e.g. `Colour(2)`
 *
 * The value is widened to the largest integer of its signedness first, so an enum based on a character type prints
 * a number too, not a character.
 *
 * @param item the enumerator to be processed
 *
 * @return the string representation
 */
template <typename T>
  requires std::is_enum_v<T> && (!Util::is_streamable<T>)
inline std::string Pretty::to_word(const T& item) {
  using Widest = std::conditional_t<std::is_signed_v<std::underlying_type_t<T>>, long long, unsigned long long>;
  return std::format("{}({})", Util::demangle(typeid(T).name()), static_cast<Widest>(std::to_underlying(item)));
}

/**
 * @brief Formats an optional as its value, or `std::nullopt` when it holds none
 *
 * An optional that holds another optional wraps the value it holds, so it reads apart from an empty one:
 * `std::optional(std::nullopt)` against `std::nullopt`.
 *
 * @param item the optional to be processed
 *
 * @return the string representation
 */
template <typename T>
inline std::string Pretty::to_word(const std::optional<T>& item) {
  if (!item.has_value()) {
    return "std::nullopt";
  }
  if constexpr (Util::is_optional_v<T>) {
    return "std::optional(" + to_word(*item) + ")";
  } else {
    return to_word(*item);
  }
}

/**
 * Formats a Matcher as a string
 *
 * Unlike the generic to_word(), this is specialized for Matchers,
 * which have a custom description associated with them for better
 * information.
 *
 * @param matcher the matcher to process
 *
 * @return a string representation of the Matcher
 */
template <typename A, typename E>
inline std::string Pretty::to_word(const Matchers::MatcherBase<A, E>& matcher) {
  std::string description = matcher.description();
  if (description.empty()) {
    return "[No description]";
  }
  return description;
}

template <typename T>
inline std::string Pretty::to_word_type(const T& item) {
  std::string word = to_word(item);
  if constexpr (Util::is_streamable<T>) {
    word += " : " + Util::demangle(typeid(T).name());
  }
  return word;
}

inline std::string Pretty::name_to_sentence(const std::string& n) const {
  return split_words(name(n));
}

inline std::string Pretty::name(const std::string& name) const {
  if (_name.empty()) {
    return last(name, ':');
  }
  return _name;
}

inline std::string Pretty::split_words(const std::string& sym) {
  return std::regex_replace(sym, std::regex("_"), " ");
}

inline std::string Pretty::underscore(const std::string& word) {
  std::string str = std::regex_replace(word, std::regex("([A-Z]+)([A-Z][a-z])"), "$1_$2");
  str = std::regex_replace(str, std::regex("([a-z\\d])([A-Z])"), "$1_$2");
  str = std::regex_replace(str, std::regex("-"), "_");
  std::ranges::transform(str, str.begin(), ::tolower);
  return str;
}

inline std::string Pretty::last(const std::string& s, const char delim) {
  std::vector<std::string> elems;
  std::stringstream ss(s);
  std::string item;
  while (getline(ss, item, delim)) {
    if (!item.empty()) {
      elems.push_back(item);
    }
  }
  return elems.back();
}

inline std::string Pretty::improve_hash_formatting(const std::string& inspect_string) {
  return std::regex_replace(inspect_string, std::regex("(\\S)=>(\\S)"), "$1 => $2");
}

/**
 * @brief Generate a string of the class and data of an object
 *
 * @param o the object to inspect
 *
 * @return the generated string
 */
template <typename O>
inline std::string Pretty::inspect_object(const O& o) {
  if constexpr (Util::is_c_string<O>) {
    // A C string is named by its type and printed quoted, or as nullptr when it is null.
    constexpr std::string_view constness = std::is_const_v<std::remove_pointer_t<std::decay_t<O>>> ? "const " : "";
    const std::string type = std::is_array_v<O> ? std::format("{}char [{}]", constness, std::extent_v<O>)
                                                : std::format("{}char *", constness);
    const auto text = string_view_of(o);
    return text ? std::format("({}) => \"{}\"", type, *text) : std::format("({}) => nullptr", type);
  } else {
    return std::format("({}) => {}", Util::demangle(typeid(o).name()), to_word(o));
  }
}

/**
 * @brief Specialization for C++ STL strings
 *
 * @param o the input string
 *
 * @return the generated string
 */
template <>
inline std::string Pretty::inspect_object<std::string>(const std::string& o) {
  return std::format("(std::string) => \"{}\"", o);
}

template <>
inline std::string Pretty::inspect_object<std::string_view>(const std::string_view& o) {
  return std::format("(std::string_view) => \"{}\"", o);
}

}  // namespace CppSpec

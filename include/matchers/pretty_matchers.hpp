/** @file */
#pragma once

#include <algorithm>
#include <regex>
#include <string>
#include <utility>
#include <vector>

#include "prettyprint.hpp"
#include "util.hpp"

namespace CppSpec {

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

  template <Util::is_streamable T>
  static std::string to_word(const T& item);

  template <typename T>
  static std::string to_word(const T& item);

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
template <Util::is_streamable T>
inline std::string Pretty::to_word(const T& item) {
  std::ostringstream oss;
  oss << item;  // use operator<< to convert and append
  return oss.str();
}

template <>
inline std::string Pretty::to_word<bool>(const bool& item) {
  return item ? "true" : "false";
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
  return std::format("({}) => {}", Util::demangle(typeid(o).name()), to_word(o));
}

/**
 * @brief Specialization for C-style strings
 *
 * @param o the input string
 *
 * @return the generated string
 */
template <>
inline std::string Pretty::inspect_object<const char*>(const char* const& o) {
  return std::format("(const char *) => \"{}\"", o);
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

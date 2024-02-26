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
  [[nodiscard]] std::string name(const std::string &name) const;
  [[nodiscard]] std::string name_to_sentence(const std::string &name) const;
  static std::string split_words(const std::string &sym);
  static std::string underscore(const std::string &camel_cased_word);
  static std::string last(const std::string &s, char delim);
  static std::string improve_hash_formatting(const std::string &inspect_string);

  template <Util::is_streamable T>
  static std::string to_word(T &item);

  template <typename T>
  static std::string to_word(T &item);

  template <typename T>
  static std::string to_word_type(T &item);

  template <typename A, typename E>
  static std::string to_word(Matchers::MatcherBase<A, E> &matcher);

  template <class T>
  static std::string to_sentence(T &item);

  template <class T>
  static std::string to_sentence(std::vector<T> &words);

  template <typename T>
  static std::string inspect_object(T &object);
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
inline std::string Pretty::to_sentence(std::vector<T> &words) {
  std::vector<std::string> my_words;
  for (T word : words) {
    my_words.push_back(to_word<T>(word));
  }
  std::stringstream ss;
  switch (my_words.size()) {
    case 0:
      return "";
    case 1:
      ss << " " << my_words[0];
      break;
    case 2:
      ss << " " << my_words[0] << " and " << my_words[1];
      break;
    default:
      ss << " ";
      for (size_t i = 0; i < my_words.size() - 1; ++i) {
        if (i != 0) ss << ", ";
        ss << my_words[i];
      }
      ss << ", and " << my_words.back();
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
inline std::string Pretty::to_sentence(T &item) {
  std::vector<T> v = {item};
  return to_sentence<T>(v);
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
inline std::string Pretty::to_word(T &item) {
  std::stringstream ss;
  ss << item;
  return ss.str();
}

template <>
inline std::string Pretty::to_word<bool>(bool &item) {
  return item ? "true" : "false";
}

template <>
inline std::string Pretty::to_word<std::true_type>(std::true_type &item) {
  return "true";
}

template <>
inline std::string Pretty::to_word<std::false_type>(std::false_type &item) {
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
inline std::string Pretty::to_word(T &item) {
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
inline std::string Pretty::to_word(Matchers::MatcherBase<A, E> &matcher) {
  std::string description = matcher.description();
  if (description.empty()) {
    return "[No description]";
  }
  return description;
}

template <typename T>
inline std::string Pretty::to_word_type(T &item) {
  std::string word = to_word(item);
  if constexpr (Util::is_streamable<T>) {
    word += " : " + Util::demangle(typeid(T).name());
  }
  return word;
}

inline std::string Pretty::name_to_sentence(const std::string &n) const { return split_words(name(n)); }

inline std::string Pretty::name(const std::string &name) const {
  if (_name.empty()) {
    return last(name, ':');
  }
  return _name;
}

inline std::string Pretty::split_words(const std::string &sym) { return std::regex_replace(sym, std::regex("_"), " "); }

inline std::string Pretty::underscore(const std::string &word) {
  std::string str = std::regex_replace(word, std::regex("([A-Z]+)([A-Z][a-z])"), "$1_$2");
  str = std::regex_replace(str, std::regex("([a-z\\d])([A-Z])"), "$1_$2");
  str = std::regex_replace(str, std::regex("-"), "_");
  std::transform(str.begin(), str.end(), str.begin(), ::tolower);
  return str;
}

inline std::string Pretty::last(const std::string &s, const char delim) {
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

inline std::string Pretty::improve_hash_formatting(const std::string &inspect_string) {
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
inline std::string Pretty::inspect_object(O &o) {
  std::stringstream ss;
  ss << "(" << Util::demangle(typeid(o).name()) << ") =>" << to_sentence(o);
  return ss.str();
}

/**
 * @brief Specialization for C-style strings
 *
 * @param o the input string
 *
 * @return the generated string
 */
template <>
inline std::string Pretty::inspect_object<const char *>(const char *&o) {
  std::stringstream ss;
  ss << "(const char *) => " << '"' << o << '"';
  return ss.str();
}

/**
 * @brief Specialization for C++ STL strings
 *
 * @param o the input string
 *
 * @return the generated string
 */
template <>
inline std::string Pretty::inspect_object<std::string>(std::string &o) {
  std::stringstream ss;
  ss << "(std::string) => " << '"' << o << '"';
  return ss.str();
}

}  // namespace CppSpec

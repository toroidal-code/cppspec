#ifndef PRETTY_MATCHERS_H
#define PRETTY_MATCHERS_H
#include <regex>
#include <algorithm>
#include "cxx-prettyprint/prettyprint.hpp"
#include "util.hpp"

namespace Matchers {
template <typename A, typename E>
class BaseMatcher;
}

/**
 * @class Pretty
 * @brief A helper base class that assists in pretty-printing various objects.
 *
 * This class provides helper string manipulation functions
 * that are used to generate human-readable information and output.
 */
struct Pretty {
  std::string _name = "";
  std::string name();
  std::string name_to_sentance();
  static std::string split_words(std::string sym);
  static std::string underscore(std::string camel_cased_word);
  static std::string last(const std::string &s, const char delim);
  static std::string improve_hash_formatting(std::string inspect_string);

  template <typename T>
  static
      typename std::enable_if<Util::is_streamable<T>::value, std::string>::type
      to_word(T &item);

  template <typename T>
  static
      typename std::enable_if<!Util::is_streamable<T>::value, std::string>::type
      to_word(T &item);

  template <typename T>
  static std::string to_word_type(T &item);

  template <typename A, typename E>
  static std::string to_word(Matchers::BaseMatcher<A, E> &matcher);

  template <class T>
  static std::string to_sentance(T &item);

  template <class T>
  static std::string to_sentance(std::vector<T> &words);
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
std::string Pretty::to_sentance(std::vector<T> &words) {
  words = std::vector<T>(words);
  std::vector<std::string> my_words;
  for (T word : words) {
    my_words.push_back(to_word(word));
  }
  std::stringstream ss;
  switch (my_words.size()) {
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
std::string Pretty::to_sentance(T &item) {
  std::vector<T> v = {item};
  return to_sentance(v);
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
typename std::enable_if<Util::is_streamable<T>::value, std::string>::type
Pretty::to_word(T &item) {
  std::stringstream ss;
  ss << item;
  return ss.str();
}

/**
 * @brief Formats an object as a string when there is no operator<<
 *
 * @param item the object to be processed
 *
 * @return the string representation
 */
template <typename T>
typename std::enable_if<!Util::is_streamable<T>::value, std::string>::type
Pretty::to_word(T &item) {
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
std::string Pretty::to_word(Matchers::BaseMatcher<A, E> &matcher) {
  std::string description = matcher.description();
  if (description.empty()) {
    return "[No description]";
  } else {
    return description;
  }
}

template <typename T>
std::string Pretty::to_word_type(T &item) {
  std::string word = to_word(item);
  if (Util::is_streamable<T>::value) {
    word += " : " + Util::demangle(typeid(T).name());
  }
  return word;
}

std::string Pretty::name_to_sentance() { return split_words(name()); }

std::string Pretty::name() {
  if (_name.empty()) {
    return last(typeid(this).name(), ':');
  } else {
    return _name;
  }
}

std::string Pretty::split_words(std::string sym) {
  std::stringstream ss;
  ss << sym;
  sym = ss.str();
  regex_replace(sym, std::regex("_"), " ");
  return sym;
}

std::string Pretty::underscore(std::string word) {
  regex_replace(word, std::regex("([A-Z]+)([A-Z][a-z])"), "$1_$2");
  regex_replace(word, std::regex("([a-z\\d])([A-Z])"), "$1_$2");
  regex_replace(word, std::regex("-"), "_");
  transform(word.begin(), word.end(), word.begin(), ::tolower);
  return word;
}

std::string Pretty::last(const std::string &s, const char delim) {
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

std::string Pretty::improve_hash_formatting(std::string inspect_string) {
  regex_replace(inspect_string, std::regex("(\\S)=>(\\S)"), "$1 => $2");
  return inspect_string;
}

#endif /* PRETTY_MATCHERS_H */

#ifndef PRETTY_MATCHERS_H
#define PRETTY_MATCHERS_H
#include "../cxx-prettyprint/prettyprint.hpp"
#include <string>
#include <sstream>
#include <vector>
#include <typeinfo>
#include <regex>
#include <algorithm>

namespace Matchers {
template <typename A, typename E>
class BaseMatcher;
}

struct Pretty {
  std::string _name = "";
  template <class T> 
  static std::string to_sentance(std::vector<T> &words);
  template <class T> 
  static std::string to_sentance(T &thing);
  template <class T> 
  static std::string to_word(T item);
  std::string name();
  std::string name_to_sentance();
  std::string split_words(std::string sym);
  std::string underscore(std::string camel_cased_word);
  std::string last(const std::string &s, const char delim);
  template <typename A, typename E>
  static std::string to_word(Matchers::BaseMatcher<A,E> &matcher);
  std::string improve_hash_formatting(std::string inspect_string);
};


template <typename T>
std::string Pretty::to_sentance(std::vector<T> &words) {
  words = std::vector<T>(words);
  std::vector<std::string> my_words; 
  for (T word: words) { my_words.push_back(to_word(word)); }
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

template <typename T>
std::string Pretty::to_sentance(T &thing) {
  std::vector<T> v = { thing };
  return to_sentance(v);
}


template <typename T>  
std::string Pretty::to_word(T item) {
  std::stringstream ss;
  ss << item;
  return ss.str();
}


template <typename A, typename E>
std::string Pretty::to_word(Matchers::BaseMatcher<A,E> &matcher) {
  std::string description = matcher.description();
  if ( description.empty()) {
    return "[No description]";
  } else {
    return description;
  }
}


std::string Pretty::name_to_sentance() {
  return split_words(name());
}



std::string Pretty::name() {
  if ( _name.empty()){ 
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
  regex_replace(word,std::regex("([A-Z]+)([A-Z][a-z])"),"$1_$2");
  regex_replace(word,std::regex("([a-z\\d])([A-Z])"),"$1_$2");
  regex_replace(word,std::regex("-"),"_");
  transform(word.begin(), word.end(), word.begin(), ::tolower);
  return word;
}

std::string Pretty::last(const std::string &s, 
                            const char delim) {
  std::vector<std::string> elems;
  std::stringstream ss(s);
  std::string item;
  while (getline(ss, item, delim)) {
    if(!item.empty()){
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





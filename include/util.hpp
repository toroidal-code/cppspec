#ifndef UTIL_H
#define UTIL_H

#ifdef __GNUG__
#include <cxxabi.h>
#endif

#include <typeinfo>
#include <iostream>
#include <sstream>
#include <string>
#include <memory>
#include <cstdlib>

namespace Util {

#ifdef __GNUG__
std::string demangle(const char *mangled) {
  int status;
  std::unique_ptr<char[], void (*)(void *)> result{
      abi::__cxa_demangle(mangled, NULL, NULL, &status), std::free};

  return (status == 0) ? result.get() : mangled;
}
#else
std::string demangle(const char *name) { return name; }
#endif

template <typename Assertion>
struct verbose_assert {
  static_assert(Assertion::value,
                "Assertion failed <see below for more information>");
  static bool const value = Assertion::value;
};

template <typename T>
class is_iterable {
  typedef char Yes;
  typedef char No[2];

  template <typename C>
  static auto test(void *) -> decltype(size_t{std::declval<C>().cbegin()},
                                       size_t{std::declval<C>().cend()}, Yes{});

  template <typename C>
  static No &test(...);

 public:
  static bool const value = sizeof(test<T>(0)) == sizeof(Yes);
};

template <typename T>
struct is_container {
  template <typename U, typename it_t = typename U::const_iterator>
  struct sfinae {
    // typedef typename U::const_iterator it_t;
    template <typename A, typename IT, IT (A::*)() const, IT (A::*)() const>
    struct type_ {};

    typedef type_<U, it_t, &U::cbegin, &U::cend> type;
  };

  template <typename U>
  static char test(typename sfinae<U>::type *);
  template <typename U>
  static long test(...);

  static bool const value = (1 == sizeof test<T>(0));
};

template <typename O>
std::string inspect_object(O o) {
  std::stringstream ss;
  ss << "(" << Util::demangle(typeid(o).name()) << ") => " << o;
  return ss.str();
}

template <>
std::string inspect_object(const char *o) {
  std::stringstream ss;
  ss << "(" << Util::demangle(typeid(o).name()) << ") => " << '"' << o << '"';
  return ss.str();
}

template <>
std::string inspect_object(std::string o) {
  std::stringstream ss;
  ss << "(" << Util::demangle(typeid(o).name()) << ") => " << '"' << o << '"';
  return ss.str();
}

template <>
std::string inspect_object(char *o) {
  std::stringstream ss;
  ss << "(" << Util::demangle(typeid(o).name()) << ") => " << '"' << o << '"';
  return ss.str();
}
}

#endif /* UTIL_H */

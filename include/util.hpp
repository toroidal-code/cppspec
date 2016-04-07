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

/**
 * Demangle a symbol into a human-readable string.
 *
 * @param mangled the mangled symbol
 *
 * @return a human-readable translation of the given symbol
 */
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

/**
 * @brief Helper class for static assertions that has a built-in error string.
 */
template <class A>
struct verbose_assert {
  static_assert(A::value, "Assertion failed (see below for more information)");
  static bool const value = A::value;
};

/**
 * @brief Functions for testing if a class is iterable
 */
namespace is_iterable_imp {
template <class C>
auto test(void *) -> decltype(void(std::declval<C>().begin()),
                              void(std::declval<C>().end()), std::true_type{});

template <class C>
auto test(...) -> std::false_type;
};

/**
 * @brief Checks whether T is an iterable type.
 *
 * Provides the member constant value which is equal
 * to `true`, if T is an iterable type. Otherwise,
 * value is equal to `false`.
 *
 * @tparam T a type to check
 */
template <class T>
class is_iterable : public decltype(is_iterable_imp::test<T>(0)) {};

/** @brief Helper variable template for is_iterable. */
template <class T>
constexpr bool is_iterable_v = is_iterable<T>::value;

/**
 * @brief Functions for testing if a class is a container
 */
namespace is_container_imp {
template <class C>
auto test(void *)
    -> decltype(std::true_type(is_iterable<C>::value),
                void(std::declval<C>().max_size()),
                void(std::declval<C>().empty()), std::true_type{});

template <class C>
auto test(...) -> std::false_type;
}

/**
 * @brief Checks whether T is a container type.
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
template <class T>
struct is_container : public decltype(is_container_imp::test<T>(0)) {};

/** @brief Helper variable template for is_container. */
template <class T>
constexpr bool is_container_v = is_container<T>::value;

/** 
 * @brief Generate a string of the class and data of an object
 *
 * @param o the object to inspect
 *
 * @return the generated string
 */ template <typename O>
std::string inspect_object(O o) {
  std::stringstream ss;
  ss << "(" << Util::demangle(typeid(o).name()) << ") => " << o;
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
std::string inspect_object(const char *o) {
  std::stringstream ss;
  ss << "(" << Util::demangle(typeid(o).name()) << ") => " << '"' << o << '"';
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
std::string inspect_object(std::string &o) {
  std::stringstream ss;
  ss << "(" << Util::demangle(typeid(o).name()) << ") => " << '"' << o << '"';
  return ss.str();
}
}

#endif /* UTIL_H */

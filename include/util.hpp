/**
 * @file
 * @brief Utility functions and classes
 */
#ifndef CPPSPEC_UTIL_HPP
#define CPPSPEC_UTIL_HPP

#ifdef __GNUG__
#include <cxxabi.h>
#include <memory>
#include <sstream>
#endif

namespace CppSpec {
namespace Util {

/**
 * Demangle a symbol into a human-readable string.
 *
 * @param mangled the mangled symbol
 *
 * @return a human-readable translation of the given symbol
 */
#ifdef __GNUG__
inline std::string demangle(const char *mangled) {
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

template <typename T>
struct is_true : public std::is_same<std::true_type, T> {};

template <typename T>
struct is_false : public std::is_same<std::false_type, T> {};

/**
 * @brief Functions for testing if a class is iterable
 */
namespace is_iterable_imp {
template <class C>
auto test(void *) -> decltype(std::declval<C>().begin(),
                              std::declval<C>().end(), std::true_type{});

template <class C>
auto test(int) -> decltype(std::declval<C>().cbegin(),
                           std::declval<C>().cend(), std::true_type{});

template <class>
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
#ifdef HAS_VARIABLE_TEMPLATES
template <class T>
constexpr bool is_iterable_v = is_iterable<T>::value;
#endif

/**
 * @brief Functions for testing if a class is a container
 */
namespace is_container_imp {
template <class C>
auto test(void *)
    -> decltype(std::declval<C>().max_size(),
                std::declval<C>().empty(), is_iterable<C>{});

template <class>
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
#ifdef HAS_VARIABLE_TEMPLATES
template <class T>
constexpr bool is_container_v = is_container<T>::value;
#endif

/**
 * @brief Functions for testing if a class can be streamed
 */
namespace is_streamable_imp {
template <typename C>
auto test(void *) -> std::is_same<
    // check for operator<< in std::ostream first (fundamental types)
    decltype(std::declval<std::ostream>().operator<<(std::declval<C>())),
    std::ostream &>;

template <class C>
auto test(int) -> std::is_same<
    // check for an operator<< via SFINAE
    decltype(operator<<(std::declval<std::ostream &>(),
                        std::declval<C const &>())),

    std::ostream &  // make sure the return type is ostream&
    >;

template <class>
auto test(...) -> std::false_type;  // fallthrough
}

/**
 * @brief Checks whether T can be streamed.
 *
 * Provides the member constant value which is equal
 * to `true`, if T is a streamable type. Otherwise,
 * value is equal to `false`.
 *
 * A streamable type is defined by having the operator<<
 * defined in scope.
 *
 * @tparam T a type to check
 */
template <class T>
struct is_streamable : public decltype(is_streamable_imp::test<T>(0)) {};

/** @brief Helper variable template for is_container. */
#ifdef HAS_VARIABLE_TEMPLATES
template <class T>
constexpr bool is_streamable_v = is_streamable<T>::value;
#endif

/**
 * @brief Functions for testing if a class can be streamed
 */
namespace is_functional_imp {

// The C++ reference says that all "functions" (not FunctionObjects) respond to
// std::is_function<T>
template <typename C>
auto test(void *) -> std::is_function<C>;

// It also says that something qualifies as a FunctionObject iff
// std::is_object<T> is true and the object responds to the call operator()
// however, we pass it over in favor of the next option.
// template <class C>
// auto test(int) -> decltype(std::is_object<C>{}, void(std::declval<C>()()),
//                           std::true_type{});

// An alternative from Stack Overflow question 18107368. Anything that could
// possibly be "functional" should be able to be cast to std::function<void()>
template <class C>
auto test(int) -> std::is_convertible<C, std::function<void()>>;

template <class>
auto test(...) -> std::false_type;  // fallthrough
}

template <class T>
struct is_functional : public decltype(is_functional_imp::test<T>(0)) {};

/** @brief Helper variable template for is_container. */
#ifdef HAS_VARIABLE_TEMPLATES
template <class T>
constexpr bool is_functional_v = is_functional<T>::value;
#endif

/**
 * @brief Generate a string of the class and data of an object
 *
 * @param o the object to inspect
 *
 * @return the generated string
 */
template <typename O>
inline std::string inspect_object(O &o) {
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
inline std::string inspect_object<const char *>(const char *&o) {
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
inline std::string inspect_object<std::string>(std::string &o) {
  std::stringstream ss;
  ss << "(std::string) => " << '"' << o << '"';
  return ss.str();
}

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
template <typename Range>
inline std::string join(Range &iterable, const std::string &separator = "") {
  std::ostringstream oss;
  typename Range::const_iterator it;
  for (it = iterable.cbegin(); it != iterable.cend();) {
    oss << *it;  // use operator<< to convert and append

    // Hacky way to get around forward_list singly-linked problem:
    // increment the iterator here, compare and then continue.
    if (++it != iterable.cend()) oss << separator;
  }
  return oss.str();
}

}  // ::Util
}  // ::CppSpec
#endif  // CPPSPEC_UTIL_HPP

/**
 * @file
 * @brief The core header file for Cppspec
 */
#ifndef CPPSPEC_HPP
#define CPPSPEC_HPP

#ifndef CPPSPEC_DEBUG
#define CPPSPEC_DEBUG false
#endif
#include "runner.hpp"

/*>>>>>>>>>>>>>>>>>>>> MACROS <<<<<<<<<<<<<<<<<<<<<<*/

#define _ [=](auto &self) mutable
#define $ [](auto &self)
#define it self.it
#define context self.template context
#define explain context  // piggybacks off of the `context` macro
#define expect self.template expect
#define is_expected self.is_expected
#define subject self.subject

#define before_all self.before_all
#define before_each self.before_each
#define after_all self.after_all
#define after_each self.after_each
#define let(name, body) auto name = self.let(body);

/*>>>>>>>>>>>>>>>>>>> TYPEDEFS <<<<<<<<<<<<<<<<<<<<<*/

typedef CppSpec::Description describe;

template <class T>
using describe_a = CppSpec::ClassDescription<T>;

template <class T>
using describe_an = CppSpec::ClassDescription<T>;

#endif  // CPPSPEC_HPP
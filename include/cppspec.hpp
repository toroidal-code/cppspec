/**
 * @file
 * @brief The core header file for Cppspec
 */
#ifndef CPPSPEC_HPP
#define CPPSPEC_HPP
#pragma once

#ifndef CPPSPEC_DEBUG
#define CPPSPEC_DEBUG false
#endif
#include "runner.hpp"

/*>>>>>>>>>>>>>>>>>>>> MACROS <<<<<<<<<<<<<<<<<<<<<<*/

// For *some* reason, MSVC++ refuses to correctly deduce the types of
// Description blocks unless the void return type is explicitly stated.
// GCC and clang have no problem with it being omitted. Weird.
#define $ [](auto &self) -> void
#define _ [=](auto &self) mutable -> void

#define it self.it
#define specify it
#ifdef _MSC_VER  // Apparently MSVC++ doesn't conform to C++14 14.2/4. Annoying.
#define context self.context
#define expect self.expect
#else
#define context self.template context
#define expect self.template expect
#endif
#define explain context  // Piggybacks off of the `context` macro

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

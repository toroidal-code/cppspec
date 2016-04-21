/**
 * @file
 * @brief The core header file for Cppspec
 */
#include "class_description.hpp"

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
#define let(name, body)              \
  auto name = make_let(#name, body); \
  self.let(#name, name);

typedef Description describe;

template <class T>
using describe_a = ClassDescription<T>;

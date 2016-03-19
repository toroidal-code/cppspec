#include "description.hpp"
#include "class_description.hpp"
#include "it.hpp"
#include "expectations/expectation.hpp"

#define _ [](auto &self)
#define it self.it
#define context self.template context
#define explain context
#define expect self.template expect
#define is_expected self.is_expected

typedef Description describe;

template <class T>
using describe_a = ClassDescription<T>;

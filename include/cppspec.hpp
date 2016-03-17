#include "description.hpp"
#include "class_description.hpp"
#include "it.hpp"
#include "expectations/expectation.hpp"

#define _ [](auto &self) -> auto
#define it self.it
#define context self.context
#define expect self.expect
#define is_expected self.is_expected
#define subject auto self = self.subject

typedef Description describe;

template <class T>
using describe_a = ClassDescription<T>;

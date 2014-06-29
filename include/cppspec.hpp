#include "description.hpp"
#include "class_description.hpp"
#include "it.hpp"
#include "expectations/expectation.hpp"

#define _blk [](auto &self) -> auto
// #define _ [](auto &self) -> auto
#define it self.it
#define context self.context
#define expect self.expect
#define subject auto self = self.subject

typedef Description describe;

template <class T>
using describe_some = ClassDescription<T>;

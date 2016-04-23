#ifndef CPPSPEC_RUNNABLE_HPP
#define CPPSPEC_RUNNABLE_HPP
#include "child.hpp"
#include "result.hpp"
#include "printer_base.hpp"

namespace CppSpec {

/**
 * @brief Abstract base class for executable objects
 */
class Runnable: public Child {
 public:
  Runnable() { };
  explicit Runnable(Child &parent) : Child(parent) { };
  explicit Runnable(Child *parent) : Child(parent) { };
  explicit Runnable(const Child *parent) : Child(parent) { };
  virtual Result run(BasePrinter &printer) = 0;
};

} // ::CppSpec
#endif // CPPSPEC_RUNNABLE_HPP

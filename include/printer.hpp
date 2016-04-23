#ifndef CPP_SPEC_PRINTER_HPP
#define CPP_SPEC_PRINTER_HPP
// the following are UBUNTU/LINUX ONLY terminal color codes.
#define RESET "\033[0m"
#define BLACK "\033[30m"              /* Black */
#define RED "\033[31m"                /* Red */
#define GREEN "\033[32m"              /* Green */
#define YELLOW "\033[33m"             /* Yellow */
#define BLUE "\033[34m"               /* Blue */
#define MAGENTA "\033[35m"            /* Magenta */
#define CYAN "\033[36m"               /* Cyan */
#define WHITE "\033[37m"              /* White */
#define BOLDBLACK "\033[1m\033[30m"   /* Bold Black */
#define BOLDRED "\033[1m\033[31m"     /* Bold Red */
#define BOLDGREEN "\033[1m\033[32m"   /* Bold Green */
#define BOLDYELLOW "\033[1m\033[33m"  /* Bold Yellow */
#define BOLDBLUE "\033[1m\033[34m"    /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m" /* Bold Magenta */
#define BOLDCYAN "\033[1m\033[36m"    /* Bold Cyan */
#define BOLDWHITE "\033[1m\033[37m"   /* Bold White */
#include <iostream>
#include "class_description.hpp"

namespace CppSpec {

class PrettyPrinter : public BasePrinter {
  int test_counter = 1;
  std::stringstream buffer;
  bool first = true;

 public:
  PrettyPrinter(const PrettyPrinter &copy)
      : BasePrinter(copy.mode),
        test_counter(copy.test_counter),
        first(copy.first) {
    buffer << copy.buffer.str();
  }

  PrettyPrinter(Mode mode) : BasePrinter(mode){};
  // template <class C>
  // void print(ClassDescription<C> &runnable);
  void print(Description &runnable) override;
  void print(BaseIt &it) override;
  void print(std::string message) override;
  void print_failure(std::string message) override;
  void flush() override;
};
// template<class C>
// void PrettyPrinter::print(ClassDescription<C> &runnable) {
//  std::cout << runnable.padding() << runnable
//}
void PrettyPrinter::print(Description &runnable) {
  switch (mode) {
    case Mode::verbose:
      if (!first && runnable.get_parent() == nullptr) std::cout << std::endl;
      std::cout << runnable.padding() << runnable.get_descr() << std::endl;
      break;
    case Mode::terse:
      break;
    case Mode::TAP:
      if (!first && runnable.get_parent() == nullptr) buffer << "\n";
      break;
  }
  if (first) this->first = false;
}
void PrettyPrinter::print(BaseIt &it) {
  std::string descr;
  switch (mode) {
    case Mode::verbose:
      std::cout << it.padding() << it.get_descr() << std::endl;
      break;
    case Mode::terse:
      std::cout << (it.get_status() ? "." : "F") << std::flush;
      break;
    case Mode::TAP:
      Description *parent = static_cast<Description *>(it.get_parent());
      while (parent != nullptr) {
        descr = parent->get_descr() + " " + descr;
        parent = static_cast<Description *>(parent->get_parent());
      }
      buffer << (it.get_status() ? "ok " : "not ok ") << test_counter << " - "
             << descr << it.get_descr() << std::endl;
      break;
  }
  test_counter++;
}
void PrettyPrinter::print(std::string message) {
  switch (mode) {
    case Mode::verbose:
      std::cout << message << std::endl;
    case Mode::terse:
      break;
    case Mode::TAP:
      buffer << message << std::endl;
      break;
  }
}
void PrettyPrinter::print_failure(std::string message) {
  // Diff diff = differ.diff(actual, expected)
  // Throwing exceptions in Ruby is totally different from C++. Gonna just
  // stdout this thing.
  switch (mode) {
    case Mode::verbose:
      std::cout << RED << message << RESET << std::endl;
    case Mode::terse:
      buffer << std::endl
             << RED << "Test number " << test_counter << " failed:" << std::endl
             << message << RESET;
    default:
      break;
  }
}

void PrettyPrinter::flush() {
  std::string str = buffer.str();
  std::stringstream ss;
  switch (mode) {
    case Mode::terse:
      if (not str.empty()) std::cout << std::endl << str << std::endl;
      break;
    case Mode::TAP:
      if (str[0] == '\n') {
        ss << str.substr(0, 1) << "1.." << test_counter - 1 << std::endl
           << str.substr(1);
      } else {
        ss << "1.." << test_counter - 1 << std::endl << str;
      }
      std::cout << ss.str() << std::flush;
      buffer = std::stringstream();
      first = false;
    default:
      test_counter = 1;
      break;
  }
}

namespace Printer {
static PrettyPrinter verbose = PrettyPrinter(PrettyPrinter::Mode::verbose);
static PrettyPrinter terse = PrettyPrinter(PrettyPrinter::Mode::terse);
static PrettyPrinter TAP = PrettyPrinter(PrettyPrinter::Mode::TAP);
}

}  // ::CppSpec
#endif  // CPP_SPEC_PRINTER_HPP

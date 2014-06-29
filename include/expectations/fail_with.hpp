#ifndef FAIL_WITH_H
#define FAIL_WITH_H
#include <string>
#include <stdexcept>
#include <iostream>
//the following are UBUNTU/LINUX ONLY terminal color codes.
#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

namespace Expectations {
template <class T> void fail_with(std::string message, T /*expected*/, T /*actual*/) {
  if (message.empty()) {
    std::cout << "Failure message is empty. Does your matcher define the "
        "appropriate failure_message[_when_negated] method to "
        "return a string?" << std::endl;
  }
  // Diff diff = differ.diff(actual, expected)
  // Throwing exceptions in Ruby is totally different from C++. Gonna just stdout this thing.
  std::cout << RED << message << RESET << std::endl;
}

void fail_with(std::string message) {
  fail_with(message, nullptr, nullptr);
}

}
#endif /* FAIL_WITH_H */

#ifndef CPPSPEC_LET_HPP
#define CPPSPEC_LET_HPP
#include "runnable.hpp"
#include "optional/optional.hpp"

namespace CppSpec {

class LetBase : public Runnable {
 protected:
  bool delivered;

 public:
  LetBase(Child &parent) : Runnable(parent), delivered(false){};
  LetBase(const LetBase &copy) : Runnable(), delivered(copy.delivered){};
  void reset() { delivered = false; }
  bool has_result() { return delivered; }
};

template <typename T>
class Let : public LetBase {
  typedef std::function<T()> block_t;
  std::experimental::optional<T> result;

  std::string name;
  block_t body;

 public:
  Let(Child &parent, std::string name, block_t body)
      : LetBase(parent), name(name), body(body){};

  T *operator->() {
    if (!delivered) run(this->get_formatter());
    return result.operator->();
  }

  T &operator*() & {
    if (!delivered) run(this->get_formatter());
    return result.operator*();
  }

  T &get_result() & { return this->operator*(); }
  std::string get_name() { return name; }

  Result run(Formatters::BaseFormatter &) override;
};

template <typename T>
Result Let<T>::run(Formatters::BaseFormatter &) {
  if (!delivered) {
    result = body();
    delivered = true;
  }
  return Result::success;
}

}  // ::CppSpec
#endif  // CPPSPEC_LET_HPP

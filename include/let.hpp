#ifndef LET_H
#define LET_H
#include "runnable.hpp"
#include "optional/optional.hpp"

class LetBase : public Runnable {
 protected:
  bool delivered;

 public:
  LetBase() : delivered(false){};
  LetBase(const LetBase& copy) : Runnable(), delivered(copy.delivered){};
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
  Let(std::string name, block_t body) : name(name), body(body){};

  T* operator->() {
    if (!delivered) run();
    return result.operator->();
  }

  T& operator*() & {
    if (!delivered) run();
    return result.operator*();
  }

  T& get_result() & { return this->operator*(); }
  std::string get_name() { return name; }

  Result run() override;
};

template <typename T>
Result Let<T>::run() {
  if (!delivered) {
    result = body();
    delivered = true;
  }
  return Result::success;
}

/**
 * @brief Object generator for Let.
 *
 * @param body the body of the let statement
 *
 * @return a new Let object
 */
template <typename T>
auto make_let(std::string name, T body) -> Let<decltype(body())> {
  return Let<decltype(body())>(name, body);
}

#endif /* LET_H */

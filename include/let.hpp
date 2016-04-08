#ifndef LET_H
#define LET_H
#include <functional>
#include "runnable.hpp"

class LetBase : public Runnable {
 protected:
  bool delivered;

 public:
  LetBase() : delivered(false){};
  void reset() { delivered = false; }
  bool has_result() { return delivered; }
  bool run() { return false; }
};

template <typename T>
class Let : public LetBase {
  typedef std::function<T()> block_t;
  T result;

  std::string name;
  block_t body;

 public:
  explicit Let(std::string name, block_t body)
      : LetBase(), name(name), body(body){};

  T &get_result() {
    run();
    return result;
  }

  std::string get_name() { return name; }

  bool run();
};

template <typename T>
bool Let<T>::run() {
  if (!delivered) {
    result = body();
    delivered = true;
  }
  return true;
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

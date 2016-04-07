#ifndef LET_H
#define LET_H
#include <functional>
#include <type_traits>
#include <iostream>
#include "runnable.hpp"

template <typename T>
class Let : public Runnable {
  typedef std::function<T()> block_t;

  std::string name;
  block_t body;

 public:
  explicit Let(std::string name, block_t body) : name(name), body(body){};
  bool run();
};

template <typename T>
bool Let<T>::run() {
  body();
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

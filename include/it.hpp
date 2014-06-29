#ifndef IT_H
#define IT_H
#include <string>
#include <functional>
#include "runnable.hpp"
#include "expectations/expectation.hpp"

class It : public Runnable {
  std::string descr;
  std::function<void(It&)> body;

 public:
  It(std::string descr, std::function<void(It&)> body)
      : descr(descr), body(body) {}
  It(std::function<void(It&)> body) : body(body) {}
  bool run();

  /**
   * expect(1)
   * expect(true)
   * expect([] -> int { return 4; })
   */
  template <class T>
  Expectations::Expectation<T> expect(T value) {
    auto expectation = Expectations::Expectation<T>(value);
    expectation.set_parent(this);
    return expectation;
  }

  /**
   * expect({1,2,3})
   */
  template <class T>
  Expectations::Expectation<std::vector<T>> expect(
      std::initializer_list<T> init_list) {
    auto expectation = Expectations::Expectation<std::vector<T>>(init_list);
    expectation.set_parent(this);
    return expectation;
  }
};

bool It::run() {
  std::cout << padding() << descr << std::endl;
  body(*this);
  return this->get_status();
}

#endif /* IT_H */

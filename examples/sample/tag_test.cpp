#include "tagged.hpp"

int func1() {
  tagged_ptr<int> i1(new int(1));
  printf("i1 = %d\n", tagged_ptr<int>::nearest());
  return 0;
}

int func0() {
  printf("i0 = %d\n", tagged_ptr<int>::nearest());
  func1();
  return 0;
}

int main() {
  char c;
  printf("start: %p\n", &c);
  tagged_ptr<int> i0(new int(0));
  printf("i0 = %d\n", tagged_ptr<int>::nearest());
  // func0();
  return 0;
}

#ifndef RUNNABLE_H
#define RUNNABLE_H
#include <string>

class Child {
  Child* parent = nullptr;
  // represents whether the children were all healthy/successful.
  bool status = true;

 public:
  virtual ~Child(){};

  std::string padding();
  Child* get_parent() { return this->parent; }
  void set_parent(Child* parent) { this->parent = parent; }
  bool get_status() { return this->status; }
  void failed() {
    this->status = false;
    // propogates the failure up the tree
    if (parent != nullptr) parent->failed();
  }
};

class Runnable : public Child {
 public:
  virtual ~Runnable() {}
  virtual bool run() = 0;
};

std::string Child::padding() {
  if (parent == nullptr) {
    return "";
  } else {
    return parent->padding() + "  ";
  }
}

#endif /* RUNNABLE_H */

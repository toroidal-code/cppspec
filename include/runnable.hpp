#ifndef RUNNABLE_H
#define RUNNABLE_H
#include <string>
#include <experimental/optional>

class Child {
  // represents whether the children were all healthy/successful.
  bool status = true;
  std::experimental::optional<Child*> parent;

 public:
  virtual ~Child(){};

  std::string padding();

  bool has_parent() { return static_cast<bool>(parent); }
  Child* get_parent() { return parent.value(); }
  void set_parent(Child* parent) { this->parent = parent; }

  bool get_status() { return this->status; }
  void failed() {
    this->status = false;
    // propogates the failure up the tree
    if (parent) parent.value()->failed();
  }
};

class Runnable : public Child {
 public:
  virtual ~Runnable() {}
  virtual bool run() = 0;
};

std::string Child::padding() {
  if (has_parent()) {
    return get_parent()->padding() + "  ";
  } else {
    return "";
  }
}

#endif /* RUNNABLE_H */

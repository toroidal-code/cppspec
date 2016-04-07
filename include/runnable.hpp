#ifndef RUNNABLE_H
#define RUNNABLE_H
#include <memory>
#include <string>

/**
 * The base class for all objects that comprise some abstract structure
 * with a nesting concept. Used to propogate ('pass') failures from leaf
 * to root without exceptions (and/or code-jumping), thus allowing
 * execution to continue virtually uninterrupted.
 */
class Child {
  std::shared_ptr<Child> parent = nullptr;  // The parent of this Child.

  // Represents whether the Child is healthy (has not failed).
  // A Child is healthy if and only if all of its children are healthy.
  // All instances of Child start out healthy.
  bool status = true;

 public:
  virtual ~Child(){};

  bool has_parent() { return (parent != nullptr); }
  std::shared_ptr<Child> get_parent() { return parent; }
  void set_parent(Child* parent) {
    this->parent = std::shared_ptr<Child>(parent);
  }
  void set_parent(std::shared_ptr<Child> parent) { this->parent = parent; }
  void set_parent(Child& parent) {
    this->parent = std::shared_ptr<Child>(&parent);
  }

  bool get_status() { return this->status; }
  void failed() {
    this->status = false;
    // propogates the failure up the tree
    if (has_parent()) this->get_parent()->failed();
  }

  std::string padding() {
    return has_parent() ? get_parent()->padding() + "  " : "";
  }
};

/**
 *
 *
 */
class Runnable : public Child {
 public:
  virtual ~Runnable() {}
  virtual bool run() = 0;
};

#endif /* RUNNABLE_H */

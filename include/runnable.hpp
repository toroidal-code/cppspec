#ifndef RUNNABLE_H
#define RUNNABLE_H
#include <memory>
#include <string>

/**
 * @brief Base class for all objects in the execution tree.
 *
 * A base class for all objects that comprise some abstract structure
 * with a nesting concept. Used to propogate ('pass') failures from leaf
 * to root without exceptions (and/or code-jumping), thus allowing
 * execution to continue virtually uninterrupted.
 */
class Child {
  // The parent of this child
  // We use a raw pointer here instead of the safer std::shared_ptr
  // due to the way that tests are inherently constructed
  // (`describe some_spec("a test", $ { ... });`). In order to use
  // a shared pointer, each object that is set as the parent must be
  // contained in a shared pointer. As tests are created by `describe ...`,
  // it is not wrapped by a shared pointer. Attempting to create this shared
  // pointer at some later time doesn't work, as it results in trying to delete
  // the current object `this` once the pointer goes out of scope.
  // Since children are always destroyed before their parents, this isn't a
  // problem anyways. In addition, any structures that are children are
  // allocated on the stack for speed reasons.
  Child* parent = nullptr;

  // Represents whether the Child is healthy (has not failed).
  // A Child is healthy if and only if all of its children are healthy.
  // All instances of Child start out healthy.
  bool status = true;

 public:
  Child(){};
  explicit Child(Child* parent) : parent(parent){};
  explicit Child(const Child* parent) : parent(const_cast<Child*>(parent)){};
  explicit Child(Child& parent) : parent(&parent){};

  /**
   * @brief Check to see if the Child has a parent.
   * @return whether or not the Child has a parent.
   */
  bool has_parent() { return static_cast<bool>(parent); }

  /** @brief Get the Child's parent. */
  Child* get_parent() { return parent; }
  const Child* get_parent() const { return const_cast<Child*>(parent); }

  /** @brief Set the Child's parent */
  void set_parent(Child* parent) { this->parent = parent; }

  /** @brief Get the status of the object (success/failure) */
  bool get_status() { return this->status; }

  /**
   * @brief Report failure to the object.
   *
   * This is propogated up the parent/child tree, so that when a child object
   * fails, the parent object is immediately updated to reflect that as well.
   */
  void failed() {
    this->status = false;
    // propogates the failure up the tree
    if (has_parent()) this->get_parent()->failed();
  }

  /**
   * @brief Get the padding (indentation) of the current object.
   * @return A string of spaces for use in pretty-printing.
   */
  std::string padding() {
    return has_parent() ? get_parent()->padding() + "  " : "";
  }
};

class Result {
  bool value;
  std::string failure_message = "";
  Result(){};
  Result(bool value, std::string failure_message = "")
      : value(value), failure_message(failure_message){};

 public:
  Result(Result const& copy)
      : value(copy.value), failure_message(copy.failure_message) {}
  operator bool() const { return value; }
  static Result success;
  static Result failure;
  static Result failure_with(std::string failure_message) {
    return Result(false, failure_message);
  }

  std::string get_message() { return failure_message; }
  bool get_status() { return value; }
};

Result Result::success = Result(true);
Result Result::failure = Result(false);

/**
 * @brief Abstract base class for executable objects
 */
class Runnable : public Child {
 public:
  Runnable(){};
  explicit Runnable(Child& parent) : Child(parent){};
  explicit Runnable(Child* parent) : Child(parent){};
  explicit Runnable(const Child* parent) : Child(parent){};
  virtual Result run() = 0;
};

#endif /* RUNNABLE_H */

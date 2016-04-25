/** @file */
#ifndef CPPSPEC_CHILD_HPP
#define CPPSPEC_CHILD_HPP
#include <iostream>
#include <typeinfo>

namespace CppSpec {

namespace Formatters {
class BaseFormatter;  // Forward declaration to allow reference
}

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
  // the root object is not wrapped by a shared pointer. Attempting to create
  // this shared pointer at some later time doesn't work, as it results in
  // trying to delete the current object `this` once the pointer goes out of
  // scope. Since children are always destroyed before their parents, this
  // isn't a problem anyways. In addition, any structures that are children
  // are allocated on the stack for speed reasons.
  Child *parent = nullptr;

  // Represents whether the Child is healthy (has not failed).
  // A Child is healthy if and only if all of its children are healthy.
  // All instances of Child start out healthy.
  bool status = true;

  // TODO: Change this to a std::unique_ptr
  Formatters::BaseFormatter *formatter = nullptr;

 public:
  // Default constructor/desctructor
  Child() = default;
  virtual ~Child() = default;

  // Move constructor/operator
  Child(Child &&) = default;
  Child &operator=(Child &&) = default;

  // Copy constructor/operator
  Child(const Child &) = default;
  Child &operator=(const Child &) = default;

  // Custom constructors
  Child(Child &parent) : parent(&parent){};
  Child(Child *parent) : parent(parent){};
  Child(const Child *parent) : parent(const_cast<Child *>(parent)){};

  /*--------- Parent helper functions -------------*/

  /** @brief Check to see if the Child has a parent. */
  const bool has_parent() { return parent != nullptr; }

  // TODO: Look in to making these references instead of pointer returns
  /** @brief Get the Child's parent. */
  Child *get_parent() { return parent; }
  const Child *get_parent() const { return const_cast<Child *>(parent); }
  template <class C>
  C get_parent_as();

  /** @brief Set the Child's parent */
  void set_parent(Child *parent) { this->parent = parent; }

  /*--------- Formatter helper functions -----------*/
  const bool has_formatter();  // Check to see if the tree has a printer
  Formatters::BaseFormatter &get_formatter();  // Get the printer from the tree
  void set_printer(Formatters::BaseFormatter &formatter) {
    this->formatter = &formatter;
  }

  /*--------- Primary member functions -------------*/

  /** @brief Get the status of the object (success/failure) */
  const bool get_status() { return this->status; }

  void failed();          // Report failure to the object.
  std::string padding();  // Calculate the padding for printing this object
};

/*>>>>>>>>>>>>>>>>>>>> Child IMPLEMENTATION <<<<<<<<<<<<<<<<<<<<<<<<<*/

/**
 * @brief Report failure to the object.
 *
 * This is propogated up the parent/child tree, so that when a child object
 * fails, the parent object is immediately updated to reflect that as well.
 */
inline void Child::failed() {
  this->status = false;
  // propogates the failure up the tree
  if (has_parent()) this->get_parent()->failed();
}

/**
 * @brief Generate padding (indentation) fore the current object.
 * @return A string of spaces for use in pretty-printing.
 */
inline std::string Child::padding() {
  return has_parent() ? get_parent()->padding() + "  " : "";
}

template <class C>
inline C Child::get_parent_as() {
  // rejected branch should get optimized out at compile-time
  if (CPPSPEC_DEBUG) {
    if (C casted = dynamic_cast<C>(get_parent())) {
      return casted;
    } else
      throw(std::bad_cast());
  } else {
    return static_cast<C>(get_parent());
  }
}

inline const bool Child::has_formatter() {
  if (this->formatter != nullptr) return true;
  if (!this->has_parent()) return false;  // base case;
  return parent->has_formatter();
}

inline Formatters::BaseFormatter &Child::get_formatter() {
  if (this->formatter != nullptr) return *formatter;
  if (!this->has_parent()) {
    std::cout << "Couldn't get printer!" << std::endl;
    throw "Couldn't get printer!";  // base case. This should never *ever*
    // happen.
  }
  return parent->get_formatter();
}

}  // ::CppSpec
#endif  // CPPSPEC_CHILD_HPP

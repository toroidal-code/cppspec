/**
 * Copyright 2016 Katherine Whitlock
 *
 * @file child.hpp
 * @brief Contains the base Child class.
 *
 * @author Katherine Whitlock (toroidalcode)
 */

#pragma once

#include <iostream>
#include <memory>
#include <string>
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

  // The global formatter when running the tests. Would typically point to one
  // of the globally instantiated formatters in formatters.hpp
  Formatters::BaseFormatter *formatter = nullptr;

 public:
  // Default constructor/destructor
  Child() = default;
  virtual ~Child() = default;

  // Move constructor/operator
  Child(Child &&) = default;
  Child &operator=(Child &&) = default;

  // Copy constructor/operator
  Child(const Child &) = default;
  Child &operator=(const Child &) = default;

  // Custom constructors
  static Child of(const Child &parent) noexcept { return Child().set_parent(&parent); }

  /*--------- Parent helper functions -------------*/

  /** @brief Check to see if the Child has a parent. */
  bool has_parent() noexcept { return parent != nullptr; }
  [[nodiscard]] bool has_parent() const noexcept { return parent != nullptr; }

  // TODO: Look in to making these references instead of pointer returns
  /** @brief Get the Child's parent. */
  [[nodiscard]] Child *get_parent() const noexcept { return parent; }

  template <class C>
  C get_parent_as() const noexcept {
    return static_cast<C>(get_parent());
  }

  /** @brief Set the Child's parent */
  Child set_parent(Child *parent) noexcept {
    this->parent = parent;
    return *this;
  }
  Child &set_parent(const Child *parent) noexcept {
    this->parent = const_cast<Child *>(parent);
    return *this;
  }

  /*--------- Formatter helper functions -----------*/
  // Check to see if the tree has a printer
  [[nodiscard]] bool has_formatter() const noexcept;

  // Get the printer from the tree
  [[nodiscard]] Formatters::BaseFormatter &get_formatter() const noexcept;

  void set_formatter(Formatters::BaseFormatter &formatter) { this->formatter = &formatter; }

  /*--------- Primary member functions -------------*/

  /** @brief Get the status of the object (success/failure) */
  [[nodiscard]] bool get_status() const noexcept { return this->status; }
  void failed() noexcept;  // Report failure to the object.

  // Calculate the padding for printing this object
  [[nodiscard]] std::string padding() const noexcept;
};

/*>>>>>>>>>>>>>>>>>>>> Child <<<<<<<<<<<<<<<<<<<<<<<<<*/

/**
 * @brief Report failure to the object.
 *
 * This is propogated up the parent/child tree, so that when a child object
 * fails, the parent object is immediately updated to reflect that as well.
 */
inline void Child::failed() noexcept {
  this->status = false;
  // propogates the failure up the tree
  if (this->has_parent()) {
    this->get_parent()->failed();
  }
}

/**
 * @brief Generate padding (indentation) fore the current object.
 * @return A string of spaces for use in pretty-printing.
 */
inline std::string Child::padding() const noexcept {
  return this->has_parent() ? this->get_parent()->padding() + "  " : "";
}

inline bool Child::has_formatter() const noexcept {
  if (this->formatter != nullptr) {
    return true;
  }
  if (!this->has_parent()) {
    return false;  // base case;
  }
  return parent->has_formatter();
}

inline Formatters::BaseFormatter &Child::get_formatter() const noexcept {
  if (this->formatter != nullptr) {
    return *formatter;
  }
  if (!this->has_parent()) {
    std::terminate();
  }
  return parent->get_formatter();
}

}  // namespace CppSpec

/** @file */
#ifndef CPPSPEC_CHILD_HPP
#define CPPSPEC_CHILD_HPP
#pragma once

#ifndef CPPSPEC_DEBUG
#define CPPSPEC_DEBUG false
#endif

#include <string>
#include <memory>
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

  Formatters::BaseFormatter *formatter = nullptr;

 public:
  // Default constructor/desctructor
  Child() = default;
  virtual ~Child() = default;

  // Move constructor/operator
  Child(Child &&) = default;
  Child &operator=(Child &&) = default;

  // Copy constructor/operator
  Child &operator=(const Child &) = default;

  // Custom constructors
  explicit Child(Child &parent) noexcept : parent(&parent) {}
  explicit Child(Child *parent) noexcept : parent(parent) {}
  explicit Child(const Child *parent) noexcept
      : parent(const_cast<Child *>(parent)) {}

  /*--------- Parent helper functions -------------*/

  /** @brief Check to see if the Child has a parent. */
  const bool has_parent() noexcept { return parent != nullptr; }

  // TODO: Look in to making these references instead of pointer returns
  /** @brief Get the Child's parent. */
  constexpr Child *get_parent() noexcept { return parent; }
  constexpr const Child *get_parent() const noexcept {
    return const_cast<Child *>(parent);
  }

  template <class C>
  constexpr C get_parent_as() noexcept {
    return static_cast<C>(get_parent());
  }

  /** @brief Set the Child's parent */
  constexpr void set_parent(Child *parent) noexcept { this->parent = parent; }
  constexpr void set_parent(const Child *parent) noexcept {
    this->parent = const_cast<Child *>(parent);
  }

  /*--------- Formatter helper functions -----------*/
  // Check to see if the tree has a printer
  constexpr const bool has_formatter() noexcept;

  // Get the printer from the tree
  constexpr Formatters::BaseFormatter &get_formatter() noexcept;

  constexpr void set_printer(const Formatters::BaseFormatter &formatter) {
    this->formatter = &const_cast<Formatters::BaseFormatter &>(formatter);
  }

  /*--------- Primary member functions -------------*/

  /** @brief Get the status of the object (success/failure) */
  constexpr const bool get_status() noexcept { return this->status; }
  constexpr const bool get_status() const noexcept { return this->status; }

  constexpr void failed() noexcept;  // Report failure to the object.

  // Calculate the padding for printing this object
  std::string padding() noexcept;
};

/*>>>>>>>>>>>>>>>>>>>> Child IMPLEMENTATION <<<<<<<<<<<<<<<<<<<<<<<<<*/

/**
 * @brief Report failure to the object.
 *
 * This is propogated up the parent/child tree, so that when a child object
 * fails, the parent object is immediately updated to reflect that as well.
 */
constexpr inline void Child::failed() noexcept {
  this->status = false;
  // propogates the failure up the tree
  if (has_parent()) this->get_parent()->failed();
}

/**
 * @brief Generate padding (indentation) fore the current object.
 * @return A string of spaces for use in pretty-printing.
 */
inline std::string Child::padding() noexcept {
  return has_parent() ? get_parent()->padding() + "  " : "";
}

constexpr inline const bool Child::has_formatter() noexcept {
  if (this->formatter != nullptr) return true;
  if (!this->has_parent()) return false;  // base case;
  return parent->has_formatter();
}

constexpr inline Formatters::BaseFormatter &Child::get_formatter() noexcept {
  if (this->formatter) return *formatter;
  if (!this->has_parent()) std::terminate();
  return parent->get_formatter();
}

}  // namespace CppSpec
#endif  // CPPSPEC_CHILD_HPP

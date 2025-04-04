#pragma once

#include <chrono>
#include <list>
#include <source_location>
#include <string>
#include "result.hpp"

namespace CppSpec {

class Result;

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
class Runnable {
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
  Runnable* parent = nullptr;

  // The source file location of the Runnable object
  std::source_location location;

  std::list<std::shared_ptr<Runnable>> children_{};  // List of children

  std::chrono::time_point<std::chrono::system_clock> start_time_;
  std::chrono::duration<double> runtime_;

 public:
  Runnable(std::source_location location) : location(location) {}

  virtual ~Runnable() = default;

  /*--------- Parent helper functions -------------*/

  /** @brief Check to see if the Runnable has a parent. */
  bool has_parent() noexcept { return parent != nullptr; }
  [[nodiscard]] bool has_parent() const noexcept { return parent != nullptr; }

  // TODO: Look in to making these references instead of pointer returns
  /** @brief Get the Runnable's parent. */
  [[nodiscard]] Runnable* get_parent() noexcept { return parent; }
  [[nodiscard]] const Runnable* get_parent() const noexcept { return parent; }

  std::list<std::shared_ptr<Runnable>>& get_children() noexcept { return children_; }
  const std::list<std::shared_ptr<Runnable>>& get_children() const noexcept { return children_; }

  template <class C>
  C* get_parent_as() noexcept {
    return static_cast<C*>(parent);
  }

  template <typename T, typename... Args>
  T* make_child(Args&&... args) {
    auto child = std::make_shared<T>(std::forward<Args>(args)...);
    auto* child_ptr = child.get();
    child->parent = this;
    children_.push_back(std::move(child));
    return child_ptr;
  }

  /*--------- Primary member functions -------------*/

  // Calculate the padding for printing this object
  [[nodiscard]] std::string padding() const noexcept;

  // Get the location of the object
  [[nodiscard]] std::source_location get_location() const noexcept { return this->location; }

  // Set the location of the object
  void set_location(std::source_location location) noexcept { this->location = location; }

  virtual void run() = 0;

  virtual void timed_run() {
    using namespace std::chrono;
    start_time_ = system_clock::now();
    time_point start_time = high_resolution_clock::now();
    run();
    time_point end = high_resolution_clock::now();
    runtime_ = end - start_time;
  }

  [[nodiscard]] std::chrono::duration<double> get_runtime() const { return runtime_; }

  [[nodiscard]] std::chrono::time_point<std::chrono::system_clock> get_start_time() const { return start_time_; }

  [[nodiscard]] virtual Result get_result() const {
    Result result = Result::success(location);
    for (auto& child : get_children()) {
      result &= child->get_result();
    }
    return result;
  }

  size_t num_tests() const noexcept {
    if (get_children().empty()) {
      return 1;  // This is a leaf node
    }

    // This is not a leaf node, so we need to count the children
    size_t count = 0;
    for (auto& child : get_children()) {
      count += child->num_tests();  // +1 for the child itself
    }
    return count;
  }

  size_t num_failures() const noexcept {
    if (get_children().empty()) {
      return this->get_result().is_failure();  // This is a leaf node
    }

    // This is not a leaf node, so we need to count the children
    size_t count = 0;
    for (auto& child : get_children()) {
      count += child->num_failures();  // +1 for the child itself
    }
    return count;
  }
};

/*>>>>>>>>>>>>>>>>>>>> Runnable <<<<<<<<<<<<<<<<<<<<<<<<<*/

/**
 * @brief Generate padding (indentation) fore the current object.
 * @return A string of spaces for use in pretty-printing.
 */
// TODO: Refactor this into Runnable::depth
inline std::string Runnable::padding() const noexcept {
  return this->has_parent() ? this->get_parent()->padding() + "  " : "";
}

}  // namespace CppSpec

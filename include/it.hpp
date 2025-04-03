/** @file */
#pragma once

#include <source_location>
#include <string>
#include <utility>
#include <vector>

#include "expectations/expectation.hpp"

namespace CppSpec {

/**
 * @brief An `it` embedded in a Description
 *
 * ItD objects are the representation of an `it` in a `describe` block (_not_ a
 * `describe_a`) the sole item they contain is the function that represents
 * the block of the `it`. ItD is derived from ItBase.
 *
 * @code
 *   describe a_thing("A Thing", $ {
 *     it("should have 4", _ { ... });
 *   });
 * @endcode
 *
 */
class ItD : public ItBase {
 public:
  using Block = std::function<void(ItD&)>;

 private:
  /** @brief The block contained in the ItD */
  const Block block;

 public:
  /**
   * @brief The primary ItD constructor
   *
   * This constructor is used for creating ItDs that are given a documentation
   * string.
   *
   * @code
   *   it("should exist", _ { ... });
   * @endcode
   *
   * @param parent the parent of the `it` (i.e. the Description object)
   * @param description the documentation string of the `it`
   * @param block the lambda/function passed to the it
   *
   * @return the constructed ItD object
   */
  ItD(Runnable& parent, std::source_location location, const char* description, Block block)
      : ItBase(parent, location, description), block(std::move(block)) {}

  /**
   * @brief The anonymous ItD constructor
   *
   * Not used frequently, as most anonymous `it`s will be ItCD objects, since
   * the syntax lends itself to this. However, the possibility of not giving a
   * documentation string to the `it` still exists, both to be consistent and
   * for cases it might want to be used.
   *
   * @code
   *   it(_ { ... });
   * @endcode
   *
   * @param parent the parent of the `it` (i.e. the Description object)
   * @param block the lambda function passed to the it
   *
   * @return the constructed ItD object
   */
  ItD(Runnable& parent, std::source_location location, Block block) : ItBase(parent, location), block(block) {}

  // implemented in description.hpp
  void run() override;
};

/**
 * @brief An `it` embedded in a ClassDescription
 *
 * ItCD objects are the representation of an `it` in a `describe_a` block. They
 * contain both the function that represents the block of the `it`, and a
 * reference to the parent ClassDescription::subject
 *
 * @tparam T the type of the subject of the parent ClassDescription
 * (ClassDescription<T>)
 */
template <typename T>
class ItCD : public ItBase {
 public:
  using Block = std::function<void(ItCD<T>&)>;

 private:
  /** @brief The block contained in the ItCD */
  const Block block;

 public:
  /**
   * @brief A reference to the parent ClassDescription's subject
   *
   * Public so that we can easily do expect(subject) without
   * needing to have a dedicated macro for accessing the subject via getter, or
   * needing to introduce parenthesis for call syntax (like `subject()`)
   */
  T& subject;

  // This is only ever instantiated by ClassDescription<T>
  ItCD(Runnable& parent, std::source_location location, T& subject, const char* description, Block block)
      : ItBase(parent, location, description), block(block), subject(subject) {}

  ItCD(Runnable& parent, std::source_location location, T& subject, Block block)
      : ItBase(parent, location), block(block), subject(subject) {}

  ExpectationValue<T> is_expected();
  void run() override;
};

/**
 * @brief A simple expect statement
 *
 * @code
 *   expect(1)
 *   expect(true)
 *   expect([] -> int { return 4; })
 * @endcode
 */
template <Util::is_not_functional T>
ExpectationValue<T> ItBase::expect(T value, std::source_location location) {
  return ExpectationValue<T>(*this, value, location);
}

template <Util::is_functional T>
ExpectationFunc<T> ItBase::expect(T block, std::source_location location) {
  return ExpectationFunc<T>(*this, block, location);
}

template <typename T>
ExpectationValue<T> ItBase::expect(Let<T>& let, std::source_location location) {
  return ExpectationValue<T>(*this, let.value(), location);
}

/**
 * @brief An expect for initializer_list subjects
 *
 * @code
 *   expect({1,2,3})
 * @endcode
 */
template <typename T>
ExpectationValue<std::initializer_list<T>> ItBase::expect(std::initializer_list<T> init_list,
                                                          std::source_location location) {
  return ExpectationValue<std::initializer_list<T>>(*this, init_list, location);
}

inline ExpectationValue<std::string> ItBase::expect(const char* str, std::source_location location) {
  return ExpectationValue<std::string>(*this, std::string(str), location);
}

}  // namespace CppSpec

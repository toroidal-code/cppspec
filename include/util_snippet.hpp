#define alias_fn(to, from)                                                \
  template <typename... Args>                                             \
  auto to(Args &&... args)->decltype(from(std::forward<Args>(args)...)) { \
    return from(std::forward<Args>(args)...);                             \
  }

#define alias_proto(to, from) \
  template <typename... Args> \
  auto to(Args &&... args)->decltype(from(std::forward<Args>(args)...))

// The parent of this child
// We use a raw pointer here instead of the safer std::shared_ptr
// due to the way that tests are inherently constructed
// (`describe some_spec("a test", $ { ... });`). In order to use
// a shared pointer, each object that is set as the

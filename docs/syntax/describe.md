Every test suite begins with either `describe` or `describe_a`.

# describe

Describes have the form of:

```cpp
describe example_spec("An example", $ { });
```

Each `describe` is a global instance of the `Description` class, the name of the spec being the name of the global variable that the test is contained in.

!!! important

    Take note of the `$`. This is used whenever you write a `describe` or a `describe_a`.


In conventional C++14, after macro-expansion the above snippet would be written as:

```cpp
Description example_spec("An example", [](&self auto) { });
```

The `Description` constructor takes two arguments: a string, and a lambda. For simplicity's sake, any lambdas passed to any C++Spec functions are referred to as "blocks", as the capture-list and arguments of the lambda are effectively never seen.

# describe_a

A `describe_a` is more complex than `describe`.

Unlike `describe` which creates instances of `Description`, `describe_a` creates instances of `ClassDescription`. `ClassDescription` is a template class, where the template's type variable is used to specialize the description and create a subject available to all statements in the description. The subject is available via the `subject` keyword.

```cpp
template <typename T>
class ClassDescription : public Description { ... };
```

Also unlike `describe`, there are two forms of `describe_a`: one where the subject is explicit, and another where it is implied.

## Explicit subject describe_a

An explicit describe_a has the subject passed into it as the first argument if there is no provided description, or after the description if there is one.

For example:

```cpp
describe_a <TestClass> tc_spec(TestClass(arg1, arg2), $ { ... });
```

and

```cpp
describe_an <AnotherTestClass> atc_spec
    ("The class AnotherTest class", AnotherTestClass(args...), $ { ... });
```

## Implied subject describe_a

```cpp
describe_a <YetAnotherTestClass> yatc_spec( $ { ... });
```

With an implied subject, the default constructor of the templated class is called to create the subject.

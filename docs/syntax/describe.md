
Every test suite begins with either `describe` or `describe_a`. 

# describe #

Describes have the form of:

```c++
describe example_spec("An example", $ { });
```

Each `describe` is a global instance of the `Description` class, the name of the spec being
the name of the global variable that the test is contained in.

__Important!__ Take note of the `$`. This is used whenever you write a `describe` or a `describe_a`.

In conventional C++, the above snippet would be written as:

```c++
Description example_spec("An example", [](&self auto) { });
```

The `Description` constructor takes two arguments: a string, and a lambda. For simplicity's sake,
any lambdas passed to any C++Spec functions are referred to as "blocks", as the capture-list 
and arguments of the lambda are effectively never seen.

# describe_a #

A `describe_a` is more complex than `describe`. 

Unlike `describe` which creates instances of `Description`, `describe_a` creates instances of 
`ClassDescription`. `ClassDescription` is a template class, where the template's type variable 
is used to specialize the description and create a subject available to all statements in the 
description.

```c++
template <typename T>
class ClassDescription : public Description { };
```

Also unlike `describe`, there are two forms of `describe_a`: one where the subject is explicit,
and another where it is implicit.

The explicit form:

```c++
describe_a <TestClass> tc_spec(TestClass(arg1, arg2), $ { });
```

The implicit form:

```c++
describe_a <AnotherTestClass> atc_spec($ { });
```


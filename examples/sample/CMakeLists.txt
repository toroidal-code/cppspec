add_executable(cppspec_sample
    example_spec.cpp
)

target_link_libraries(cppspec_sample c++spec)

add_executable(jasmine_intro jasmine_intro.cpp)
target_link_libraries(jasmine_intro c++spec)


set_target_properties(cppspec_sample jasmine_intro PROPERTIES
  CXX_STANDARD 23
  CXX_STANDARD_REQUIRED YES
)

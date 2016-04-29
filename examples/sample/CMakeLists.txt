add_executable(cppspec_sample
    example_spec.cpp
)

target_link_libraries(cppspec_sample c++spec)

add_executable(jasmine_intro jasmine_intro.cpp)
target_link_libraries(jasmine_intro c++spec)
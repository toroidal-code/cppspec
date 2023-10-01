cmake_minimum_required(VERSION 3.27 FATAL_ERROR)
if(IWYU)
  find_program(iwyu_path NAMES include-what-you-use iwyu)
endif(IWYU)

project(c++spec)

set(CMAKE_COLOR_DIAGNOSTICS ON)

add_library(c++spec INTERFACE)
target_include_directories(c++spec INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
  $<INSTALL_INTERFACE:include/cppspec/> # <prefix>/include/cppspec
)

FILE(GLOB_RECURSE c++spec_headers ${CMAKE_CURRENT_LIST_DIR}/include/*.hpp)
target_precompile_headers(c++spec INTERFACE
  ${c++spec_headers}
)

option(BUILD_TESTS "Build tests." OFF)

if(BUILD_TESTS)
  # enable_testing()

  # Tests
  add_subdirectory(spec)
endif(BUILD_TESTS)

option(BUILD_EXAMPLES "Build examples." OFF)

if(BUILD_EXAMPLES)
  add_subdirectory(examples)

  if(iwyu_path)
    set_property(TARGET cppspec_sample PROPERTY CXX_INCLUDE_WHAT_YOU_USE ${iwyu_path})
  endif()
endif(BUILD_EXAMPLES)

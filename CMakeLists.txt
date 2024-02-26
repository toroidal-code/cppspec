cmake_minimum_required(VERSION 3.27 FATAL_ERROR)

project(c++spec)

set(CMAKE_COLOR_DIAGNOSTICS ON)

cmake_policy(SET CMP0135 NEW)

include(FetchContent)

FetchContent_Declare(argparse
  URL https://github.com/p-ranav/argparse/archive/refs/tags/v3.0.tar.gz
)
FetchContent_MakeAvailable(argparse)

FetchContent_Declare(cxx-prettyprint
  URL https://github.com/louisdx/cxx-prettyprint/archive/refs/heads/master.tar.gz
)
FetchContent_MakeAvailable(cxx-prettyprint)

add_library(cxx-prettyprint INTERFACE)
target_sources(cxx-prettyprint INTERFACE ${cxx-prettyprint_SOURCE_DIR}/prettyprint.hpp)
target_include_directories(cxx-prettyprint INTERFACE ${cxx-prettyprint_SOURCE_DIR})

add_library(c++spec INTERFACE)
target_include_directories(c++spec INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
  $<INSTALL_INTERFACE:include/cppspec/> # <prefix>/include/cppspec
)
target_link_libraries(c++spec INTERFACE
  cxx-prettyprint
  argparse
)

FILE(GLOB_RECURSE c++spec_headers ${CMAKE_CURRENT_LIST_DIR}/include/*.hpp)

option(CPPSPEC_PRECOMPILE_HEADERS "Precompile the C++Spec headers")
if(CPPSPEC_PRECOMPILE_HEADERS)
  target_precompile_headers(c++spec INTERFACE
    ${c++spec_headers}
  )
endif()

# HELPERS

# Add spec
function(add_spec source_file)
  cmake_path(GET source_file STEM spec_name)
  add_executable(${spec_name} ${source_file})
  target_link_libraries(${spec_name} c++spec)

  set_target_properties(${spec_name} PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED YES
  )
  add_test(NAME ${spec_name} COMMAND ${spec_name} --verbose)
endfunction(add_spec)

# Discover Specs
function(discover_specs spec_folder)
  file(GLOB_RECURSE specs ${spec_folder}/*_spec.cpp)

  foreach(spec IN LISTS specs)
    add_spec(${spec})
  endforeach()
endfunction(discover_specs)

# OPTIONS
option(CPPSPEC_BUILD_TESTS "Build C++Spec tests")
option(CPPSPEC_BUILD_EXAMPLES "Build C++Spec examples")
option(CPPSPEC_BUILD_DOCS "Build C++Spec documentation")

if(CPPSPEC_BUILD_TESTS)
  enable_testing()

  # Tests
  discover_specs(spec)
endif(CPPSPEC_BUILD_TESTS)

if(CPPSPEC_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif(CPPSPEC_BUILD_EXAMPLES)

# #### Documentation generation #######
if(CPPSPEC_BUILD_DOCS)
  find_package(Doxygen
    OPTIONAL_COMPONENTS dot mscgen dia
  )

  if(DOXYGEN_FOUND)
    if(NOT ${DOXYGEN_HAVE_DOT})
      message(
        "Can't find GraphViz DOT tool for generating images."
        "Make sure it's on your PATH or install GraphViz")
    endif()

    FetchContent_Declare(doxygen-awesome-css
      URL https://github.com/jothepro/doxygen-awesome-css/archive/refs/tags/v2.2.1.tar.gz
      URL_HASH MD5=340d3a206794ac01a91791c2a513991f
    )
    FetchContent_MakeAvailable(doxygen-awesome-css)

    set(DOXYGEN_PROJECT_NAME "C++Spec")
    set(DOXYGEN_PROJECT_BRIEF "BDD testing for C++")
    set(DOXYGEN_RECURSIVE YES)
    set(DOXYGEN_EXAMPLE_RECURSIVE YES)
    set(DOXYGEN_EXCLUDE_PATTERNS "*/cxx-prettyprint/*")
    set(DOXYGEN_NUM_PROC_THREADS ${HOST_NUM_CORES})

    # From doxygen-awesome
    set(DOXYGEN_GENERATE_TREEVIEW YES)
    set(DOXYGEN_DISABLE_INDEX NO)
    set(DOXYGEN_FULL_SIDEBAR NO)
    set(DOXYGEN_HTML_COLORSTYLE LIGHT)
    set(DOXYGEN_HTML_EXTRA_STYLESHEET "${doxygen-awesome-css_SOURCE_DIR}/doxygen-awesome.css")

    set(DOXYGEN_USE_MDFILE_AS_MAINPAGE README.md)

    file(GLOB markdown_SOURCES *.md)

    doxygen_add_docs(doxygen ${markdown_SOURCES} include)
  else(DOXYGEN_FOUND)
    message(WARNING
      "Doxygen needs to be installed to generate documentation."
      "Please install from https://github.com/doxygen/doxygen/releases")
  endif(DOXYGEN_FOUND)
endif(CPPSPEC_BUILD_DOCS)

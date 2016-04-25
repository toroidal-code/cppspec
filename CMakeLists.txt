project( c++spec )

if ( IWYU )
  cmake_minimum_required( VERSION 3.3 FATAL_ERROR )
  find_program(iwyu_path NAMES include-what-you-use iwyu)
else ()
  cmake_minimum_required( VERSION 3.0 )
endif ( IWYU )

set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y -Wall -Werror")

set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")
include(cotire)

add_library( c++spec INTERFACE )
target_include_directories( c++spec INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
  $<INSTALL_INTERFACE:include/>  # <prefix>/include/
)

option ( BUILD_TESTS "Build tests." OFF )
if ( BUILD_TESTS )
  # enable_testing()

  # Tests
  add_subdirectory( spec )
endif ( BUILD_TESTS )

option( BUILD_EXAMPLES "Build examples." OFF )
if ( BUILD_EXAMPLES )
  add_subdirectory( examples )
  if(iwyu_path)
    set_property(TARGET cppspec_sample PROPERTY CXX_INCLUDE_WHAT_YOU_USE ${iwyu_path})
  endif()
endif ( BUILD_EXAMPLES )

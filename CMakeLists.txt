project( c++spec )

cmake_minimum_required( VERSION 3.0 )

set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y -Wall -Werror")

add_library( c++spec INTERFACE )
target_include_directories( c++spec INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
  $<INSTALL_INTERFACE:include/>  # <prefix>/include/
)

option ( BUILD_TESTING "Build tests." OFF )
if ( BUILD_TESTING )
  add_subdirectory( spec )
endif ( BUILD_TESTING )

option( BUILD_EXAMPLES "Build examples." OFF )
if ( BUILD_EXAMPLES )
  add_subdirectory( examples )
endif ( BUILD_EXAMPLES )

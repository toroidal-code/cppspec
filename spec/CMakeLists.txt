add_executable( describe_a_spec describe_a_spec.cpp )
target_link_libraries( describe_a_spec c++spec ${CMAKE_THREAD_LIBS_INIT} )
add_test ( describe_a_spec describe_a_spec )



add_custom_target ( spec ${CMAKE_COMMAND} -E env CTEST_OUTPUT_ON_FAILURE=1
  ${CMAKE_CTEST_COMMAND} -C $<CONFIG> -j 4 WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

#add_executable( describe_spec describe_spec.cpp )
#target_link_libraries( describe_spec c++spec ${CMAKE_THREAD_LIBS_INIT} )
#add_test ( describe_spec describe_spec )
#
#add_executable( describe_a_spec describe_a_spec.cpp )
#target_link_libraries( describe_a_spec c++spec ${CMAKE_THREAD_LIBS_INIT} )
#add_test ( describe_a_spec describe_a_spec )
#
#add_executable( expectation_spec expectations/expectation_spec.cpp )
#target_link_libraries( expectation_spec c++spec ${CMAKE_THREAD_LIBS_INIT} )
#add_test ( expectation_spec expectation_spec )
#
#add_executable( be_between_spec matchers/be_between_spec.cpp )
#target_link_libraries( be_between_spec c++spec ${CMAKE_THREAD_LIBS_INIT} )
#add_test ( be_between_spec be_between_spec )
#
#add_executable( be_within_spec matchers/be_within_spec.cpp )
#target_link_libraries( be_within_spec c++spec ${CMAKE_THREAD_LIBS_INIT} )
#add_test ( be_within_spec be_within_spec )
#
#add_executable( equal_spec matchers/equal_spec.cpp )
#target_link_libraries( equal_spec c++spec ${CMAKE_THREAD_LIBS_INIT} )
#add_test ( equal_spec equal_spec )

add_executable( spec_runner runner.cpp
        expectations/expectation_spec.cpp
        matchers/be_between_spec.cpp
        matchers/be_within_spec.cpp
        matchers/equal_spec.cpp
        describe_a_spec.cpp
        describe_spec.cpp
        )
target_link_libraries( spec_runner c++spec ${CMAKE_THREAD_LIBS_INIT})
cotire ( spec_runner )

#add_custom_target ( spec ${CMAKE_COMMAND} -E env CTEST_OUTPUT_ON_FAILURE=1
#  ${CMAKE_CTEST_COMMAND} -C $<CONFIG> -j 4 WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
#
#add_custom_target( spec
#        spec_runner
#        COMMENT Running specs
#        )

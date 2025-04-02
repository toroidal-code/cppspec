if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  add_compile_options(-Wno-unused-parameter -Wno-missing-template-arg-list-after-template-kw -Wno-missing-template-keyword -Wno-unknown-warning-option)
endif()

discover_specs(${CMAKE_CURRENT_SOURCE_DIR})

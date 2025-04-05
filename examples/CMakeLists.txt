set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_subdirectory(sample)
#add_subdirectory(skip)
#add_subdirectory(xml_output)

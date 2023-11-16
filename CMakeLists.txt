cmake_minimum_required(VERSION 3.27)
project(curry_function)

set(CMAKE_CXX_STANDARD 20)

add_library(curry_function INTERFACE curry_function.hpp)

target_include_directories(curry_function INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

find_package(Boost REQUIRED COMPONENTS unit_test_framework)
add_executable(tests tests.cpp)

if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(tests PRIVATE
            -Wall -Wextra -Werror
            -Weffc++ -Wpedantic -Wconversion -Wshadow -Wfloat-equal -Woverloaded-virtual -Wunreachable-code
    )
endif ()

if (MSVC)
    target_compile_options(tests PRIVATE /W4 /WX)
endif ()

target_link_libraries(tests Boost::unit_test_framework)
target_include_directories(tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
enable_testing()
add_test(NAME Tests COMMAND tests)

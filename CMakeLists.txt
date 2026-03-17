cmake_minimum_required(VERSION 3.14)

project(OS_3)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_library(os3_core
    src/threads.cpp
    src/winapi_error.cpp
    src/marker_logic.cpp
)

target_include_directories(os3_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

add_executable(OS_3
    src/main.cpp
)

target_include_directories(OS_3 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(OS_3 PRIVATE os3_core)

add_executable(os3_tests
    tests/basic_tests.cpp
)

target_include_directories(os3_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(os3_tests PRIVATE
    os3_core
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(os3_tests)
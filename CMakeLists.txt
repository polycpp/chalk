cmake_minimum_required(VERSION 3.20)
project(polycpp_chalk VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

set(POLYCPP_BUILD_TESTS OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    polycpp
    GIT_REPOSITORY git@github.com:enricohuang/polycpp.git
    GIT_TAG        master
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(polycpp)

add_library(polycpp_chalk
    src/chalk.cpp
)
target_link_libraries(polycpp_chalk PUBLIC polycpp)
target_include_directories(polycpp_chalk PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

add_library(polycpp::chalk ALIAS polycpp_chalk)

option(POLYCPP_CHALK_BUILD_TESTS "Build tests" ON)
if(POLYCPP_CHALK_BUILD_TESTS)
    enable_testing()
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY git@github.com:google/googletest.git
        GIT_TAG        v1.15.2
    )
    FetchContent_MakeAvailable(googletest)

    add_executable(test_chalk tests/test_chalk.cpp)
    target_link_libraries(test_chalk PRIVATE polycpp_chalk GTest::gtest_main)
    include(GoogleTest)
    gtest_discover_tests(test_chalk)
endif()

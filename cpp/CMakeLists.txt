cmake_minimum_required(VERSION 3.14)
project(GildedRose)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.16.0.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
  TLS_VERIFY OFF
)
FetchContent_MakeAvailable(googletest)

FetchContent_Declare(ApprovalTests
    GIT_REPOSITORY https://github.com/approvals/ApprovalTests.cpp.git
    GIT_TAG v.10.13.0)
FetchContent_MakeAvailable(ApprovalTests)



# Library
add_library(gilded_rose src/GildedRose.cpp)
target_include_directories(gilded_rose PUBLIC include)

# Tests
enable_testing()
add_executable(gilded_rose_test test/GildedRoseTest.cpp)
target_link_libraries(gilded_rose_test gilded_rose GTest::gtest_main ApprovalTests::ApprovalTests)

include(GoogleTest)
gtest_discover_tests(gilded_rose_test)

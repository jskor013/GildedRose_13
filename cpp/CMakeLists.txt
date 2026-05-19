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

# Library
add_library(gilded_rose src/GildedRose.cpp)
target_include_directories(gilded_rose PUBLIC include)

# Tests
enable_testing()
add_executable(gilded_rose_test test/GildedRoseTest.cpp)
target_link_libraries(gilded_rose_test gilded_rose GTest::gtest_main)

add_executable(gilded_rose_approval_test test/GildedRoseApprovalTest.cpp)
target_link_libraries(gilded_rose_approval_test gilded_rose GTest::gtest_main)
target_compile_definitions(gilded_rose_approval_test PRIVATE
  GILDED_ROSE_APPROVAL_DIR="${CMAKE_CURRENT_SOURCE_DIR}/test/approval"
  GILDED_ROSE_RECEIVED_DIR="${CMAKE_CURRENT_BINARY_DIR}/approval"
)

include(GoogleTest)
gtest_discover_tests(gilded_rose_test)
gtest_discover_tests(gilded_rose_approval_test)

cmake_minimum_required(VERSION 3.14)

project(GildedRoseCpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# ─────────────────────────────────────────────
# GoogleTest
# ─────────────────────────────────────────────

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.16.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    TLS_VERIFY OFF
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

# ─────────────────────────────────────────────
# ApprovalTests.cpp
# ─────────────────────────────────────────────

FetchContent_Declare(
    ApprovalTests
    GIT_REPOSITORY https://github.com/approvals/ApprovalTests.cpp.git
    GIT_TAG v.10.13.0
)

FetchContent_MakeAvailable(ApprovalTests)

# ─────────────────────────────────────────────
# Library
# ─────────────────────────────────────────────

add_library(
    gilded_rose
    src/GildedRose.cpp
)

target_include_directories(
    gilded_rose
    PUBLIC include
)

# ─────────────────────────────────────────────
# Test Executable
# ─────────────────────────────────────────────

enable_testing()

add_executable(
    gilded_rose_test
    test/GildedRoseTest.cpp
)

# ─────────────────────────────────────────────
# Link Libraries
# ─────────────────────────────────────────────

target_link_libraries(
    gilded_rose_test
    PRIVATE
        gilded_rose
        GTest::gtest_main
        GTest::gmock
        ApprovalTests::ApprovalTests
)

# ─────────────────────────────────────────────
# GoogleTest Discovery
# ─────────────────────────────────────────────

include(GoogleTest)

gtest_discover_tests(gilded_rose_test)
cmake_minimum_required(VERSION 3.14)

project(TupleMakerTests LANGUAGES CXX)

include(../cmake/project-is-top-level.cmake)
include(../cmake/folders.cmake)

# ---- Dependencies ----

if(PROJECT_IS_TOP_LEVEL)
  find_package(TupleMaker REQUIRED)
  enable_testing()
endif()

include(FetchContent)

set(INSTALL_GTEST OFF)
FetchContent_Declare(GTest
  GIT_REPOSITORY https://github.com/google/googletest
  GIT_TAG v1.15.2
)
FetchContent_MakeAvailable(GTest)

# ---- Tests ----

add_executable(TupleMaker_test source/TupleMaker_test.cpp)
target_link_libraries(TupleMaker_test PRIVATE TupleMaker::TupleMaker GTest::gtest_main)
target_compile_features(TupleMaker_test PRIVATE cxx_std_17)

add_test(NAME TupleMaker_test COMMAND TupleMaker_test)

# ---- End-of-file commands ----

include(GoogleTest)
gtest_discover_tests(TupleMaker_test)

add_folders(Test)

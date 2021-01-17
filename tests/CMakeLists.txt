cmake_minimum_required (VERSION 3.10)

# Prepare "Catch" library for other executables
set(CATCH_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/catch)
add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE ${CATCH_INCLUDE_DIR})

# Define test source files
set(TEST_SOURCES "fixedpoint_test.cpp")

# Compile with c++98
set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Make test executable
add_executable(fixedpoint_test ${TEST_SOURCES})
target_compile_definitions(fixedpoint_test PRIVATE WITH_FLOAT_CONVERSION)
target_link_libraries(fixedpoint_test PRIVATE fixedpoint Catch)

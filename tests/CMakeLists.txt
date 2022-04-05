cmake_minimum_required (VERSION 3.10)

set(CATCH_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/catch)
add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE ${CATCH_INCLUDE_DIR})

add_executable(scaled_int_test "scaled_int_test.cpp")

# Compile with c++98
set_target_properties(scaled_int_test PROPERTIES CXX_STANDARD 98)
target_compile_definitions(scaled_int_test PRIVATE WITH_FLOAT_CONVERSION)
target_link_libraries(scaled_int_test PRIVATE fixedpoint Catch)

add_test(NAME ScaledIntTest COMMAND scaled_int_test)
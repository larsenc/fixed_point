cmake_minimum_required (VERSION 3.10)

add_executable(simple_arithmetics "simple_arithmetics.cpp")

# Compile with c++98
set_target_properties(simple_arithmetics PROPERTIES CXX_STANDARD 98)
target_compile_definitions(simple_arithmetics PRIVATE WITH_FLOAT_CONVERSION)
target_link_libraries(simple_arithmetics PRIVATE fixed_point Catch)
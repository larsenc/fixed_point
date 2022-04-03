cmake_minimum_required(VERSION 3.10)

set(CURRENT_VERSION 0.1)

project(fixedpoint VERSION ${CURRENT_VERSION} LANGUAGES CXX)

add_subdirectory(include)
add_subdirectory(tests)
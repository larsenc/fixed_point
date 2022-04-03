cmake_minimum_required (VERSION 3.10)

add_library(fixedpoint INTERFACE)

target_include_directories(fixedpoint INTERFACE ${PROJECT_SOURCE_DIR}/include)

source_group(TREE ${PROJECT_SOURCE_DIR}/include PREFIX "Header Files" FILES ${HEADER_LIST})
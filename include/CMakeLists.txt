cmake_minimum_required (VERSION 3.10)

add_library(fixedpoint INTERFACE)
target_include_directories(fixedpoint INTERFACE ${PROJECT_SOURCE_DIR}/include)
target_compile_features(fixedpoint INTERFACE cxx_std_98)
source_group(TREE ${PROJECT_SOURCE_DIR}/include PREFIX "Header Files" FILES ${HEADER_LIST})
cmake_minimum_required(VERSION 3.10)

project(fixedpoint VERSION 0.1 LANGUAGES CXX)

add_library(fixedpoint INTERFACE)
target_include_directories(fixedpoint INTERFACE
	$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include>
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
	"${PROJECT_BINARY_DIR}/fixedpointConfigVersion.cmake"
	VERSION ${CURRENT_VERSION}
	COMPATIBILITY AnyNewerVersion
)

install(TARGETS fixedpoint
	EXPORT fixedpointTargets
	LIBRARY DESTINATION lib COMPONENT Runtime
	ARCHIVE DESTINATION lib COMPONENT Development
	RUNTIME DESTINATION bin COMPONENT Runtime
	PUBLIC_HEADER DESTINATION include COMPONENT Development
	BUNDLE DESTINATION bin COMPONENT Runtime
)

include(CMakePackageConfigHelpers)
configure_package_config_file(
	"${PROJECT_SOURCE_DIR}/cmake/fixedpointConfig.cmake.in"
	"${PROJECT_BINARY_DIR}/fixedpointConfig.cmake"
	INSTALL_DESTINATION lib/cmake/fixedpoint
)

install(EXPORT fixedpointTargets
	DESTINATION lib/cmake/fixedpoint
)

install(FILES
	"${PROJECT_BINARY_DIR}/fixedpointConfigVersion.cmake"
	"${PROJECT_BINARY_DIR}/fixedpointConfig.cmake"
	DESTINATION lib/cmake/fixedpoint
)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/
	DESTINATION include
)

option(FIXEDPOINT_BUILD_TESTS "Build fixedpoint tests" ON)
if(FIXEDPOINT_BUILD_TESTS)
	add_subdirectory(tests)
endif()

export(PACKAGE fixedpoint)
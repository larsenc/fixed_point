cmake_minimum_required(VERSION 3.10)

project("fixed_point"
	VERSION 0.1
	LANGUAGES CXX
	DESCRIPTION "A header-only c++ library that provides a fixed point type, scaled_int, that follows the Q format."
	HOMEPAGE_URL "https://github.com/larsenc/fixed_point")

option(FIXED_POINT_BUILD_TESTS "Build fixed point tests" ON)
option(FIXED_POINT_BUILD_EXAMPLES "Build fixed point examples" ON)

include(GNUInstallDirs)

add_library(fixed_point INTERFACE)

target_include_directories(fixed_point
	INTERFACE	$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
				$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

if(FIXED_POINT_BUILD_TESTS)
	add_subdirectory(tests)
endif()

if(FIXED_POINT_BUILD_EXAMPLES)
	add_subdirectory(examples)
endif()

install(TARGETS fixed_point
	EXPORT ${PROJECT_NAME}_Targets
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
	"fixed_pointConfigVersion.cmake"
	VERSION ${PROJECT_VERSION}
	COMPATIBILITY SameMajorVersion
)

include(CMakePackageConfigHelpers)
configure_package_config_file(
	"${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
	"${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
	INSTALL_DESTINATION
	${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake
)

install(EXPORT ${PROJECT_NAME}_Targets
	FILE ${PROJECT_NAME}Targets.cmake
	NAMESPACE ${PROJECT_NAME}::
	DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake
)

install(FILES	"${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
				"${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
	DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake
)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/
	DESTINATION include
)
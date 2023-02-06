cmake_minimum_required(VERSION 3.23.0)
project(stacktrace VERSION 0.1.0)

include(cmake/warnings.cmake)
include(cmake/packaging.cmake)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

# option(STACKTRACE_TESTS "Build tests for stacktrace" OFF)
option(STACKTRACE_EXAMPLES "Build examples for stacktrace" OFF)

add_library(stacktrace INTERFACE)
add_library(stacktrace::stacktrace ALIAS stacktrace)
file(GLOB_RECURSE stacktrace_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} CONFIGURE_DEPENDS "src/*.hpp")
target_sources(stacktrace INTERFACE FILE_SET HEADERS BASE_DIRS src FILES ${stacktrace_HEADERS})
target_include_directories(stacktrace INTERFACE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
	$<INSTALL_INTERFACE:include>
)

if(WIN32)
	target_link_libraries(stacktrace INTERFACE dbghelp)
else()
	find_package(libunwind REQUIRED)
	find_package(elfutils REQUIRED)

	target_link_libraries(stacktrace INTERFACE elfutils::dw unwind)
endif()

target_compile_features(stacktrace INTERFACE cxx_std_17)
target_package(stacktrace)

# if(STACKTRACE_TESTS)
# 	add_executable(stacktrace_tests "tests/main.cpp")
# 	target_compile_warnings(stacktrace_tests treat_as_errors gnu_all gnu_extra ms_1)
# 	target_link_libraries(stacktrace_tests PRIVATE stacktrace)

# 	install(TARGETS stacktrace_tests)
# endif()

if(STACKTRACE_EXAMPLES)
	file(GLOB STACKTRACE_EXAMPLE_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} CONFIGURE_DEPENDS "examples/*.cpp")

	foreach(STACKTRACE_EXAMPLE_SOURCE IN LISTS STACKTRACE_EXAMPLE_SOURCES)
		cmake_path(GET STACKTRACE_EXAMPLE_SOURCE STEM STACKTRACE_EXAMPLE_TARGET_SUFFIX)
		set(STACKTRACE_EXAMPLE_TARGET "stacktrace_example_${STACKTRACE_EXAMPLE_TARGET_SUFFIX}")

		add_executable(${STACKTRACE_EXAMPLE_TARGET} "${STACKTRACE_EXAMPLE_SOURCE}")
		set_target_properties(${STACKTRACE_EXAMPLE_TARGET} PROPERTIES ENABLE_EXPORTS ON)
		target_link_libraries(${STACKTRACE_EXAMPLE_TARGET} PRIVATE stacktrace)

		install(TARGETS ${STACKTRACE_EXAMPLE_TARGET})
		message(STATUS "Found example: ${STACKTRACE_EXAMPLE_TARGET_SUFFIX}")
	endforeach()
endif()
